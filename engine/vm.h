/**
 * ============================================================================
 * 50 Days of C - Domain Engine: Stack-based Bytecode Virtual Machine
 * File: engine/vm.h
 * Standard: ISO C99 / C11
 * ============================================================================
 * Description:
 *   Header definitions for a high-performance stack-based Bytecode Virtual
 *   Machine. Features custom opcodes, constant pool management, operand stack
 *   tracking, instruction disassembly, and runtime safety checks.
 * ============================================================================
 */

#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 1. OPCODES & INSTRUCTION SET ARCHITECTURE
 * ============================================================================
 */
typedef enum {
    OP_HALT = 0,    /**< Terminate execution successfully */
    OP_PUSH,        /**< Push constant from constant pool: OP_PUSH <uint16_t index> */
    OP_ADD,         /**< Binary addition: pop b, pop a, push (a + b) */
    OP_SUB,         /**< Binary subtraction: pop b, pop a, push (a - b) */
    OP_MUL,         /**< Binary multiplication: pop b, pop a, push (a * b) */
    OP_DIV,         /**< Binary division: pop b, pop a, push (a / b) with zero check */
    OP_NEG,         /**< Unary negation: pop a, push (-a) */
    OP_PRINT,       /**< Print top of stack to stdout */
    OP_POP,         /**< Discard top of stack */
    OP_DUP          /**< Duplicate top of stack */
} Opcode;

/**
 * Value representation for VM operands.
 * Uses double-precision IEEE 754 floating point numbers for universal math.
 */
typedef double Value;

/* ============================================================================
 * 2. RUNTIME ERROR & STATUS CODES
 * ============================================================================
 */
typedef enum {
    VM_OK = 0,
    VM_ERR_STACK_OVERFLOW,
    VM_ERR_STACK_UNDERFLOW,
    VM_ERR_DIVISION_BY_ZERO,
    VM_ERR_UNKNOWN_OPCODE,
    VM_ERR_INVALID_OPERAND,
    VM_ERR_OUT_OF_BOUNDS,
    VM_ERR_NULL_POINTER
} VMResult;

/* ============================================================================
 * 3. BYTECODE CHUNK & CONSTANT POOL
 * ============================================================================
 */
typedef struct {
    uint8_t *code;           /**< Raw bytecode instruction stream */
    size_t   code_count;     /**< Number of instruction bytes emitted */
    size_t   code_capacity;  /**< Allocated capacity of instruction stream */

    Value   *constants;      /**< Constant pool for numeric literals */
    size_t   const_count;    /**< Number of constants stored */
    size_t   const_capacity; /**< Allocated capacity of constant pool */
} Chunk;

/* ============================================================================
 * 4. VIRTUAL MACHINE CORE STATE
 * ============================================================================
 */
typedef struct {
    Value       *stack;          /**< Operand stack buffer */
    size_t       stack_capacity; /**< Max stack depth */
    size_t       sp;             /**< Stack pointer (points to next free slot) */
    const uint8_t *ip;           /**< Instruction pointer */
    bool         trace_mode;     /**< Enable opcode disassembly tracing */
    Value        last_result;    /**< Cached value from last OP_PRINT or OP_POP */
} VM;

/* ============================================================================
 * 5. CHUNK API PROTOTYPES
 * ============================================================================
 */

/** Initialize an empty bytecode chunk */
void chunk_init(Chunk *chunk);

/** Release memory allocated to bytecode chunk */
void chunk_free(Chunk *chunk);

/** Append a single byte to the bytecode instruction stream */
void chunk_write_byte(Chunk *chunk, uint8_t byte);

/** Append a 16-bit unsigned integer in big-endian order */
void chunk_write_uint16(Chunk *chunk, uint16_t value);

/** Add a numeric constant to the pool, returns its index */
size_t chunk_add_constant(Chunk *chunk, Value value);

/** Disassemble an entire chunk to stdout for debugging and verification */
void chunk_disassemble(const Chunk *chunk, const char *name);

/** Disassemble a single instruction at offset, returns next instruction offset */
size_t instruction_disassemble(const Chunk *chunk, size_t offset);

/* ============================================================================
 * 6. VIRTUAL MACHINE LIFECYCLE & EXECUTION API
 * ============================================================================
 */

/** Allocate and initialize a new Virtual Machine instance */
VM *vm_create(size_t stack_capacity);

/** Reset the VM stack and state without reallocating */
void vm_reset(VM *vm);

/** Execute a compiled bytecode chunk */
VMResult vm_run(VM *vm, const Chunk *chunk);

/** Deallocate VM instance and its operand stack */
void vm_free(VM *vm);

/** Stack manipulation helpers */
bool vm_push(VM *vm, Value value);
bool vm_pop(VM *vm, Value *out_value);
bool vm_peek(const VM *vm, size_t distance, Value *out_value);

/** Get a human-readable string for VMResult status codes */
const char *vm_result_to_string(VMResult result);

#ifdef __cplusplus
}
#endif

#endif /* VM_H */
