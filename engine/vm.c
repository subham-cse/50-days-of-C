/**
 * ============================================================================
 * 50 Days of C - Domain Engine: Stack-based Bytecode Virtual Machine
 * File: engine/vm.c
 * Standard: ISO C99 / C11
 * ============================================================================
 * Description:
 *   Implementation of the stack-based Bytecode Virtual Machine engine.
 *   Provides instruction dispatch, constant pool lookup, runtime bounds checking,
 *   opcode disassembly, and arithmetic error handling.
 * ============================================================================
 */

#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INITIAL_CHUNK_CAPACITY 16
#define FLOAT_ZERO_EPSILON     1e-14

/* ============================================================================
 * 1. BYTECODE CHUNK IMPLEMENTATION
 * ============================================================================
 */

void chunk_init(Chunk *chunk)
{
    if (!chunk) return;
    chunk->code = NULL;
    chunk->code_count = 0;
    chunk->code_capacity = 0;
    chunk->constants = NULL;
    chunk->const_count = 0;
    chunk->const_capacity = 0;
}

void chunk_free(Chunk *chunk)
{
    if (!chunk) return;
    free(chunk->code);
    free(chunk->constants);
    chunk_init(chunk);
}

void chunk_write_byte(Chunk *chunk, uint8_t byte)
{
    if (!chunk) return;
    if (chunk->code_count + 1 > chunk->code_capacity) {
        size_t new_cap = chunk->code_capacity < INITIAL_CHUNK_CAPACITY 
                         ? INITIAL_CHUNK_CAPACITY 
                         : chunk->code_capacity * 2;
        uint8_t *new_code = (uint8_t *)realloc(chunk->code, new_cap * sizeof(uint8_t));
        if (!new_code) {
            fprintf(stderr, "[VM FATAL] Out of memory expanding bytecode buffer.\n");
            exit(EXIT_FAILURE);
        }
        chunk->code = new_code;
        chunk->code_capacity = new_cap;
    }
    chunk->code[chunk->code_count++] = byte;
}

void chunk_write_uint16(Chunk *chunk, uint16_t value)
{
    /* Emit high byte then low byte (Big-endian) */
    chunk_write_byte(chunk, (uint8_t)((value >> 8) & 0xFF));
    chunk_write_byte(chunk, (uint8_t)(value & 0xFF));
}

size_t chunk_add_constant(Chunk *chunk, Value value)
{
    if (!chunk) return 0;
    /* Check if constant already exists in pool to deduplicate */
    for (size_t i = 0; i < chunk->const_count; i++) {
        if (fabs(chunk->constants[i] - value) < FLOAT_ZERO_EPSILON) {
            return i;
        }
    }

    if (chunk->const_count + 1 > chunk->const_capacity) {
        size_t new_cap = chunk->const_capacity < INITIAL_CHUNK_CAPACITY 
                         ? INITIAL_CHUNK_CAPACITY 
                         : chunk->const_capacity * 2;
        Value *new_consts = (Value *)realloc(chunk->constants, new_cap * sizeof(Value));
        if (!new_consts) {
            fprintf(stderr, "[VM FATAL] Out of memory expanding constant pool.\n");
            exit(EXIT_FAILURE);
        }
        chunk->constants = new_consts;
        chunk->const_capacity = new_cap;
    }
    size_t index = chunk->const_count++;
    chunk->constants[index] = value;
    return index;
}

/* ============================================================================
 * 2. DISASSEMBLER
 * ============================================================================
 */

size_t instruction_disassemble(const Chunk *chunk, size_t offset)
{
    if (!chunk || offset >= chunk->code_count) return offset;

    printf("%04zu  ", offset);
    uint8_t instruction = chunk->code[offset];

    switch (instruction) {
        case OP_HALT:
            printf("OP_HALT\n");
            return offset + 1;

        case OP_PUSH: {
            if (offset + 2 >= chunk->code_count) {
                printf("OP_PUSH (Truncated operand)\n");
                return offset + 1;
            }
            uint16_t const_idx = (uint16_t)((chunk->code[offset + 1] << 8) | chunk->code[offset + 2]);
            if (const_idx < chunk->const_count) {
                printf("OP_PUSH   const[%u] -> %g\n", const_idx, chunk->constants[const_idx]);
            } else {
                printf("OP_PUSH   const[%u] (Out of bounds)\n", const_idx);
            }
            return offset + 3;
        }

        case OP_ADD:
            printf("OP_ADD\n");
            return offset + 1;

        case OP_SUB:
            printf("OP_SUB\n");
            return offset + 1;

        case OP_MUL:
            printf("OP_MUL\n");
            return offset + 1;

        case OP_DIV:
            printf("OP_DIV\n");
            return offset + 1;

        case OP_NEG:
            printf("OP_NEG\n");
            return offset + 1;

        case OP_PRINT:
            printf("OP_PRINT\n");
            return offset + 1;

        case OP_POP:
            printf("OP_POP\n");
            return offset + 1;

        case OP_DUP:
            printf("OP_DUP\n");
            return offset + 1;

        default:
            printf("UNKNOWN_OPCODE (0x%02X)\n", instruction);
            return offset + 1;
    }
}

void chunk_disassemble(const Chunk *chunk, const char *name)
{
    printf("=== Bytecode Disassembly [%s] ===\n", name ? name : "Anonymous");
    if (!chunk || chunk->code_count == 0) {
        printf("  (Empty bytecode chunk)\n");
        return;
    }

    size_t offset = 0;
    while (offset < chunk->code_count) {
        offset = instruction_disassemble(chunk, offset);
    }
    printf("=== End Disassembly (Total bytes: %zu, Constants: %zu) ===\n\n", 
           chunk->code_count, chunk->const_count);
}

/* ============================================================================
 * 3. VIRTUAL MACHINE LIFECYCLE
 * ============================================================================
 */

VM *vm_create(size_t stack_capacity)
{
    if (stack_capacity == 0) {
        stack_capacity = 256; /* Default safe stack depth */
    }

    VM *vm = (VM *)malloc(sizeof(VM));
    if (!vm) return NULL;

    vm->stack = (Value *)malloc(stack_capacity * sizeof(Value));
    if (!vm->stack) {
        free(vm);
        return NULL;
    }

    vm->stack_capacity = stack_capacity;
    vm->sp = 0;
    vm->ip = NULL;
    vm->trace_mode = false;
    vm->last_result = 0.0;

    return vm;
}

void vm_free(VM *vm)
{
    if (!vm) return;
    free(vm->stack);
    free(vm);
}

void vm_reset(VM *vm)
{
    if (!vm) return;
    vm->sp = 0;
    vm->ip = NULL;
    vm->last_result = 0.0;
}

/* ============================================================================
 * 4. STACK HELPERS
 * ============================================================================
 */

bool vm_push(VM *vm, Value value)
{
    if (!vm || vm->sp >= vm->stack_capacity) {
        return false;
    }
    vm->stack[vm->sp++] = value;
    return true;
}

bool vm_pop(VM *vm, Value *out_value)
{
    if (!vm || vm->sp == 0) {
        return false;
    }
    Value val = vm->stack[--vm->sp];
    if (out_value) {
        *out_value = val;
    }
    return true;
}

bool vm_peek(const VM *vm, size_t distance, Value *out_value)
{
    if (!vm || distance >= vm->sp) {
        return false;
    }
    if (out_value) {
        *out_value = vm->stack[vm->sp - 1 - distance];
    }
    return true;
}

/* ============================================================================
 * 5. EXECUTION ENGINE
 * ============================================================================
 */

VMResult vm_run(VM *vm, const Chunk *chunk)
{
    if (!vm || !chunk) return VM_ERR_NULL_POINTER;
    if (chunk->code_count == 0) return VM_OK;

    vm->ip = chunk->code;
    const uint8_t *code_end = chunk->code + chunk->code_count;

    while (vm->ip < code_end) {
        if (vm->trace_mode) {
            size_t curr_offset = (size_t)(vm->ip - chunk->code);
            instruction_disassemble(chunk, curr_offset);
        }

        uint8_t instruction = *vm->ip++;

        switch (instruction) {
            case OP_HALT:
                return VM_OK;

            case OP_PUSH: {
                if (vm->ip + 1 >= code_end) {
                    return VM_ERR_OUT_OF_BOUNDS;
                }
                uint16_t const_idx = (uint16_t)((*vm->ip << 8) | *(vm->ip + 1));
                vm->ip += 2;

                if (const_idx >= chunk->const_count) {
                    return VM_ERR_INVALID_OPERAND;
                }

                Value val = chunk->constants[const_idx];
                if (!vm_push(vm, val)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_ADD: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (!vm_push(vm, a + b)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_SUB: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (!vm_push(vm, a - b)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_MUL: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (!vm_push(vm, a * b)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_DIV: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (fabs(b) < FLOAT_ZERO_EPSILON) {
                    return VM_ERR_DIVISION_BY_ZERO;
                }
                if (!vm_push(vm, a / b)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_NEG: {
                Value a;
                if (!vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (!vm_push(vm, -a)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            case OP_PRINT: {
                Value a;
                if (!vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                vm->last_result = a;
                printf("[VM_ENGINE] Execution Result: %g\n", a);
                break;
            }

            case OP_POP: {
                Value a;
                if (!vm_pop(vm, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                vm->last_result = a;
                break;
            }

            case OP_DUP: {
                Value a;
                if (!vm_peek(vm, 0, &a)) {
                    return VM_ERR_STACK_UNDERFLOW;
                }
                if (!vm_push(vm, a)) {
                    return VM_ERR_STACK_OVERFLOW;
                }
                break;
            }

            default:
                return VM_ERR_UNKNOWN_OPCODE;
        }
    }

    return VM_OK;
}

const char *vm_result_to_string(VMResult result)
{
    switch (result) {
        case VM_OK:                   return "VM_OK (Success)";
        case VM_ERR_STACK_OVERFLOW:   return "VM_ERR_STACK_OVERFLOW (Operand stack exceeded capacity)";
        case VM_ERR_STACK_UNDERFLOW:  return "VM_ERR_STACK_UNDERFLOW (Insufficient operands on stack)";
        case VM_ERR_DIVISION_BY_ZERO: return "VM_ERR_DIVISION_BY_ZERO (Division by zero detected)";
        case VM_ERR_UNKNOWN_OPCODE:   return "VM_ERR_UNKNOWN_OPCODE (Unrecognized instruction)";
        case VM_ERR_INVALID_OPERAND:  return "VM_ERR_INVALID_OPERAND (Constant pool index out of range)";
        case VM_ERR_OUT_OF_BOUNDS:    return "VM_ERR_OUT_OF_BOUNDS (Instruction pointer exceeded code chunk)";
        case VM_ERR_NULL_POINTER:     return "VM_ERR_NULL_POINTER (Null pointer passed to VM)";
        default:                      return "VM_ERR_UNKNOWN";
    }
}
