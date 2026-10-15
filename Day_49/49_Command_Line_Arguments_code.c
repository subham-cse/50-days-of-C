/**
 * ============================================================================
 * DAY 49: Domain Engine: Recursive-Descent AST Parser & Bytecode Virtual Machine
 * File: Day_49/49_Command_Line_Arguments_code.c
 * Standard: ISO C99 / C11
 * ============================================================================
 * Description:
 *   A self-contained, production-grade domain engine implementing:
 *     1. Lexical Scanner / Tokenizer for arithmetic expressions.
 *     2. Abstract Syntax Tree (AST) node hierarchy and memory management.
 *     3. Recursive-descent grammar parser with operator precedence.
 *     4. Bytecode compiler emitting a custom 8-bit Instruction Set Architecture.
 *     5. Stack-based Bytecode Virtual Machine with bounds checks & error traps.
 *     6. Full CLI argument evaluation driver via argc / argv with fallback suite.
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 -pthread -O2 49_Command_Line_Arguments_code.c -lm -o day49_vm
 *
 * Execution:
 *   ./day49_vm                             # Run internal verification test suite
 *   ./day49_vm "(10 + 20) * 3 / 2 - 5"     # Evaluate custom CLI expression
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#define INITIAL_CHUNK_CAPACITY 16
#define FLOAT_EPSILON          1e-14

/* ============================================================================
 * 1. INSTRUCTION SET ARCHITECTURE & OPCODES
 * ============================================================================
 */

typedef enum {
    OP_HALT = 0,    /**< Terminate execution successfully */
    OP_PUSH,        /**< Push constant pool value onto stack: OP_PUSH <uint16_t index> */
    OP_ADD,         /**< Binary addition: pop b, pop a, push (a + b) */
    OP_SUB,         /**< Binary subtraction: pop b, pop a, push (a - b) */
    OP_MUL,         /**< Binary multiplication: pop b, pop a, push (a * b) */
    OP_DIV,         /**< Binary division: pop b, pop a, push (a / b) with zero trap */
    OP_NEG,         /**< Unary negation: pop a, push (-a) */
    OP_PRINT,       /**< Pop top value and print to stdout */
    OP_POP,         /**< Discard top value */
    OP_DUP          /**< Duplicate top value */
} Opcode;

typedef double Value;

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
 * 2. BYTECODE CHUNK & CONSTANT POOL
 * ============================================================================
 */

typedef struct {
    uint8_t *code;
    size_t   code_count;
    size_t   code_capacity;

    Value   *constants;
    size_t   const_count;
    size_t   const_capacity;
} Chunk;

void chunk_init(Chunk *chunk) {
    if (!chunk) return;
    chunk->code = NULL;
    chunk->code_count = 0;
    chunk->code_capacity = 0;
    chunk->constants = NULL;
    chunk->const_count = 0;
    chunk->const_capacity = 0;
}

void chunk_free(Chunk *chunk) {
    if (!chunk) return;
    free(chunk->code);
    free(chunk->constants);
    chunk_init(chunk);
}

void chunk_write_byte(Chunk *chunk, uint8_t byte) {
    if (chunk->code_count + 1 > chunk->code_capacity) {
        size_t new_cap = chunk->code_capacity < INITIAL_CHUNK_CAPACITY 
                         ? INITIAL_CHUNK_CAPACITY 
                         : chunk->code_capacity * 2;
        uint8_t *new_code = (uint8_t *)realloc(chunk->code, new_cap * sizeof(uint8_t));
        if (!new_code) {
            fprintf(stderr, "[FATAL] Out of memory expanding bytecode chunk.\n");
            exit(EXIT_FAILURE);
        }
        chunk->code = new_code;
        chunk->code_capacity = new_cap;
    }
    chunk->code[chunk->code_count++] = byte;
}

void chunk_write_uint16(Chunk *chunk, uint16_t value) {
    chunk_write_byte(chunk, (uint8_t)((value >> 8) & 0xFF));
    chunk_write_byte(chunk, (uint8_t)(value & 0xFF));
}

size_t chunk_add_constant(Chunk *chunk, Value value) {
    for (size_t i = 0; i < chunk->const_count; i++) {
        if (fabs(chunk->constants[i] - value) < FLOAT_EPSILON) {
            return i;
        }
    }
    if (chunk->const_count + 1 > chunk->const_capacity) {
        size_t new_cap = chunk->const_capacity < INITIAL_CHUNK_CAPACITY 
                         ? INITIAL_CHUNK_CAPACITY 
                         : chunk->const_capacity * 2;
        Value *new_consts = (Value *)realloc(chunk->constants, new_cap * sizeof(Value));
        if (!new_consts) {
            fprintf(stderr, "[FATAL] Out of memory expanding constant pool.\n");
            exit(EXIT_FAILURE);
        }
        chunk->constants = new_consts;
        chunk->const_capacity = new_cap;
    }
    size_t idx = chunk->const_count++;
    chunk->constants[idx] = value;
    return idx;
}

void chunk_disassemble(const Chunk *chunk, const char *title) {
    printf("\n--- Bytecode Disassembly: %s ---\n", title ? title : "Chunk");
    size_t offset = 0;
    while (offset < chunk->code_count) {
        printf("%04zu  ", offset);
        uint8_t op = chunk->code[offset++];
        switch (op) {
            case OP_HALT:  printf("OP_HALT\n"); break;
            case OP_ADD:   printf("OP_ADD\n"); break;
            case OP_SUB:   printf("OP_SUB\n"); break;
            case OP_MUL:   printf("OP_MUL\n"); break;
            case OP_DIV:   printf("OP_DIV\n"); break;
            case OP_NEG:   printf("OP_NEG\n"); break;
            case OP_PRINT: printf("OP_PRINT\n"); break;
            case OP_POP:   printf("OP_POP\n"); break;
            case OP_DUP:   printf("OP_DUP\n"); break;
            case OP_PUSH: {
                if (offset + 1 >= chunk->code_count) {
                    printf("OP_PUSH [truncated]\n");
                    break;
                }
                uint16_t c_idx = (uint16_t)((chunk->code[offset] << 8) | chunk->code[offset + 1]);
                offset += 2;
                if (c_idx < chunk->const_count) {
                    printf("OP_PUSH   const[%u] -> %g\n", c_idx, chunk->constants[c_idx]);
                } else {
                    printf("OP_PUSH   const[%u] [out of bounds]\n", c_idx);
                }
                break;
            }
            default:
                printf("UNKNOWN_OPCODE (0x%02X)\n", op);
                break;
        }
    }
    printf("--- End Disassembly (%zu bytes, %zu constants) ---\n\n",
           chunk->code_count, chunk->const_count);
}

/* ============================================================================
 * 3. STACK-BASED BYTECODE VIRTUAL MACHINE
 * ============================================================================
 */

typedef struct {
    Value       *stack;
    size_t       stack_capacity;
    size_t       sp;
    const uint8_t *ip;
    Value        last_result;
} VM;

VM *vm_create(size_t stack_capacity) {
    if (stack_capacity == 0) stack_capacity = 256;
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
    vm->last_result = 0.0;
    return vm;
}

void vm_free(VM *vm) {
    if (!vm) return;
    free(vm->stack);
    free(vm);
}

void vm_reset(VM *vm) {
    if (!vm) return;
    vm->sp = 0;
    vm->ip = NULL;
    vm->last_result = 0.0;
}

static bool vm_push(VM *vm, Value val) {
    if (vm->sp >= vm->stack_capacity) return false;
    vm->stack[vm->sp++] = val;
    return true;
}

static bool vm_pop(VM *vm, Value *out_val) {
    if (vm->sp == 0) return false;
    Value v = vm->stack[--vm->sp];
    if (out_val) *out_val = v;
    return true;
}

VMResult vm_run(VM *vm, const Chunk *chunk) {
    if (!vm || !chunk) return VM_ERR_NULL_POINTER;
    if (chunk->code_count == 0) return VM_OK;

    vm->ip = chunk->code;
    const uint8_t *code_end = chunk->code + chunk->code_count;

    while (vm->ip < code_end) {
        uint8_t instruction = *vm->ip++;
        switch (instruction) {
            case OP_HALT:
                return VM_OK;

            case OP_PUSH: {
                if (vm->ip + 1 >= code_end) return VM_ERR_OUT_OF_BOUNDS;
                uint16_t c_idx = (uint16_t)((*vm->ip << 8) | *(vm->ip + 1));
                vm->ip += 2;
                if (c_idx >= chunk->const_count) return VM_ERR_INVALID_OPERAND;
                if (!vm_push(vm, chunk->constants[c_idx])) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_ADD: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                if (!vm_push(vm, a + b)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_SUB: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                if (!vm_push(vm, a - b)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_MUL: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                if (!vm_push(vm, a * b)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_DIV: {
                Value b, a;
                if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                if (fabs(b) < FLOAT_EPSILON) return VM_ERR_DIVISION_BY_ZERO;
                if (!vm_push(vm, a / b)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_NEG: {
                Value a;
                if (!vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                if (!vm_push(vm, -a)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            case OP_PRINT: {
                Value a;
                if (!vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                vm->last_result = a;
                printf("[VM_ENGINE OP_PRINT] => %g\n", a);
                break;
            }

            case OP_POP: {
                Value a;
                if (!vm_pop(vm, &a)) return VM_ERR_STACK_UNDERFLOW;
                vm->last_result = a;
                break;
            }

            case OP_DUP: {
                if (vm->sp == 0) return VM_ERR_STACK_UNDERFLOW;
                Value top = vm->stack[vm->sp - 1];
                if (!vm_push(vm, top)) return VM_ERR_STACK_OVERFLOW;
                break;
            }

            default:
                return VM_ERR_UNKNOWN_OPCODE;
        }
    }
    return VM_OK;
}

const char *vm_result_str(VMResult res) {
    switch (res) {
        case VM_OK:                   return "VM_OK (Success)";
        case VM_ERR_STACK_OVERFLOW:   return "VM_ERR_STACK_OVERFLOW";
        case VM_ERR_STACK_UNDERFLOW:  return "VM_ERR_STACK_UNDERFLOW";
        case VM_ERR_DIVISION_BY_ZERO: return "VM_ERR_DIVISION_BY_ZERO";
        case VM_ERR_UNKNOWN_OPCODE:   return "VM_ERR_UNKNOWN_OPCODE";
        case VM_ERR_INVALID_OPERAND:  return "VM_ERR_INVALID_OPERAND";
        case VM_ERR_OUT_OF_BOUNDS:    return "VM_ERR_OUT_OF_BOUNDS";
        case VM_ERR_NULL_POINTER:     return "VM_ERR_NULL_POINTER";
        default:                      return "VM_ERR_UNKNOWN";
    }
}

/* ============================================================================
 * 4. ABSTRACT SYNTAX TREE (AST) DEFINITION
 * ============================================================================
 */

typedef enum {
    AST_NUMBER,
    AST_BINARY_OP,
    AST_UNARY_OP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        Value number_value;
        struct {
            char op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;
        struct {
            char op;
            struct ASTNode *operand;
        } unary;
    } as;
} ASTNode;

ASTNode *ast_create_number(Value val) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_NUMBER;
    node->as.number_value = val;
    return node;
}

ASTNode *ast_create_binary(char op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_BINARY_OP;
    node->as.binary.op = op;
    node->as.binary.left = left;
    node->as.binary.right = right;
    return node;
}

ASTNode *ast_create_unary(char op, ASTNode *operand) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_UNARY_OP;
    node->as.unary.op = op;
    node->as.unary.operand = operand;
    return node;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    if (node->type == AST_BINARY_OP) {
        ast_free(node->as.binary.left);
        ast_free(node->as.binary.right);
    } else if (node->type == AST_UNARY_OP) {
        ast_free(node->as.unary.operand);
    }
    free(node);
}

void ast_print(const ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case AST_NUMBER:
            printf("Number: %g\n", node->as.number_value);
            break;
        case AST_BINARY_OP:
            printf("BinaryOp ('%c')\n", node->as.binary.op);
            ast_print(node->as.binary.left, indent + 1);
            ast_print(node->as.binary.right, indent + 1);
            break;
        case AST_UNARY_OP:
            printf("UnaryOp ('%c')\n", node->as.unary.op);
            ast_print(node->as.unary.operand, indent + 1);
            break;
    }
}

/* ============================================================================
 * 5. LEXER / TOKENIZER & RECURSIVE-DESCENT PARSER
 * ============================================================================
 */

typedef enum {
    TOK_NUMBER,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef struct {
    TokenType   type;
    const char *start;
    size_t      length;
    Value       value;
} Token;

typedef struct {
    const char *source;
    const char *current;
    Token       current_token;
    Token       previous_token;
    bool        had_error;
} Parser;

static void lexer_advance(Parser *p) {
    while (*p->current && isspace((unsigned char)*p->current)) {
        p->current++;
    }

    p->previous_token = p->current_token;

    if (*p->current == '\0') {
        p->current_token.type = TOK_EOF;
        p->current_token.start = p->current;
        p->current_token.length = 0;
        return;
    }

    char c = *p->current++;

    if (isdigit((unsigned char)c) || (c == '.' && isdigit((unsigned char)*p->current))) {
        const char *num_start = p->current - 1;
        while (isdigit((unsigned char)*p->current)) p->current++;
        if (*p->current == '.' && isdigit((unsigned char)p->current[1])) {
            p->current++;
            while (isdigit((unsigned char)*p->current)) p->current++;
        }
        char *end_ptr = NULL;
        Value val = strtod(num_start, &end_ptr);
        p->current_token.type = TOK_NUMBER;
        p->current_token.start = num_start;
        p->current_token.length = (size_t)(p->current - num_start);
        p->current_token.value = val;
        return;
    }

    p->current_token.start = p->current - 1;
    p->current_token.length = 1;
    p->current_token.value = 0.0;

    switch (c) {
        case '+': p->current_token.type = TOK_PLUS; return;
        case '-': p->current_token.type = TOK_MINUS; return;
        case '*': p->current_token.type = TOK_STAR; return;
        case '/': p->current_token.type = TOK_SLASH; return;
        case '(': p->current_token.type = TOK_LPAREN; return;
        case ')': p->current_token.type = TOK_RPAREN; return;
        default:
            p->current_token.type = TOK_ERROR;
            p->had_error = true;
            return;
    }
}

static bool match_token(Parser *p, TokenType type) {
    if (p->current_token.type == type) {
        lexer_advance(p);
        return true;
    }
    return false;
}

/* Recursive grammar rule declarations */
static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_term(Parser *p);
static ASTNode *parse_factor(Parser *p);
static ASTNode *parse_primary(Parser *p);

static ASTNode *parse_primary(Parser *p) {
    if (match_token(p, TOK_NUMBER)) {
        return ast_create_number(p->previous_token.value);
    }
    if (match_token(p, TOK_LPAREN)) {
        ASTNode *expr = parse_expression(p);
        if (!match_token(p, TOK_RPAREN)) {
            fprintf(stderr, "[PARSER ERROR] Expected ')' after grouped expression.\n");
            p->had_error = true;
            ast_free(expr);
            return NULL;
        }
        return expr;
    }
    fprintf(stderr, "[PARSER ERROR] Expected numeric literal or '(' sub-expression.\n");
    p->had_error = true;
    return NULL;
}

static ASTNode *parse_factor(Parser *p) {
    if (match_token(p, TOK_MINUS)) {
        ASTNode *operand = parse_factor(p);
        return ast_create_unary('-', operand);
    }
    if (match_token(p, TOK_PLUS)) {
        return parse_factor(p);
    }
    return parse_primary(p);
}

static ASTNode *parse_term(Parser *p) {
    ASTNode *left = parse_factor(p);
    while (p->current_token.type == TOK_STAR || p->current_token.type == TOK_SLASH) {
        char op = (p->current_token.type == TOK_STAR) ? '*' : '/';
        lexer_advance(p);
        ASTNode *right = parse_factor(p);
        left = ast_create_binary(op, left, right);
    }
    return left;
}

static ASTNode *parse_expression(Parser *p) {
    ASTNode *left = parse_term(p);
    while (p->current_token.type == TOK_PLUS || p->current_token.type == TOK_MINUS) {
        char op = (p->current_token.type == TOK_PLUS) ? '+' : '-';
        lexer_advance(p);
        ASTNode *right = parse_term(p);
        left = ast_create_binary(op, left, right);
    }
    return left;
}

ASTNode *ast_parse(const char *source) {
    Parser parser;
    parser.source = source;
    parser.current = source;
    parser.had_error = false;
    lexer_advance(&parser);

    ASTNode *root = parse_expression(&parser);
    if (parser.had_error || parser.current_token.type != TOK_EOF) {
        ast_free(root);
        return NULL;
    }
    return root;
}

/* ============================================================================
 * 6. AST-TO-BYTECODE COMPILER EMITTER
 * ============================================================================
 */

bool compile_ast_to_bytecode(const ASTNode *node, Chunk *chunk) {
    if (!node || !chunk) return false;

    switch (node->type) {
        case AST_NUMBER: {
            size_t c_idx = chunk_add_constant(chunk, node->as.number_value);
            if (c_idx > 65535) return false;
            chunk_write_byte(chunk, OP_PUSH);
            chunk_write_uint16(chunk, (uint16_t)c_idx);
            return true;
        }
        case AST_UNARY_OP: {
            if (!compile_ast_to_bytecode(node->as.unary.operand, chunk)) return false;
            if (node->as.unary.op == '-') {
                chunk_write_byte(chunk, OP_NEG);
            }
            return true;
        }
        case AST_BINARY_OP: {
            if (!compile_ast_to_bytecode(node->as.binary.left, chunk)) return false;
            if (!compile_ast_to_bytecode(node->as.binary.right, chunk)) return false;
            switch (node->as.binary.op) {
                case '+': chunk_write_byte(chunk, OP_ADD); break;
                case '-': chunk_write_byte(chunk, OP_SUB); break;
                case '*': chunk_write_byte(chunk, OP_MUL); break;
                case '/': chunk_write_byte(chunk, OP_DIV); break;
                default: return false;
            }
            return true;
        }
    }
    return false;
}

/* ============================================================================
 * 7. COMPLETE PIPELINE EVALUATOR
 * ============================================================================
 */

bool evaluate_expression(const char *expr_str, Value *out_result, bool verbose) {
    if (verbose) {
        printf("----------------------------------------------------------------------\n");
        printf("Evaluating Expression: \"%s\"\n", expr_str);
        printf("----------------------------------------------------------------------\n");
    }

    /* Step 1: Lexical analysis and AST parsing */
    ASTNode *root = ast_parse(expr_str);
    if (!root) {
        fprintf(stderr, "[ERROR] Syntax parsing failed for expression: \"%s\"\n", expr_str);
        return false;
    }

    if (verbose) {
        printf("[AST Representation]:\n");
        ast_print(root, 1);
    }

    /* Step 2: Bytecode compilation */
    Chunk chunk;
    chunk_init(&chunk);

    if (!compile_ast_to_bytecode(root, &chunk)) {
        fprintf(stderr, "[ERROR] Bytecode compilation failed.\n");
        ast_free(root);
        chunk_free(&chunk);
        return false;
    }

    /* Emit print and halt instructions */
    chunk_write_byte(&chunk, OP_PRINT);
    chunk_write_byte(&chunk, OP_HALT);

    if (verbose) {
        chunk_disassemble(&chunk, expr_str);
    }

    /* Step 3: Virtual Machine execution */
    VM *vm = vm_create(256);
    if (!vm) {
        ast_free(root);
        chunk_free(&chunk);
        return false;
    }

    VMResult res = vm_run(vm, &chunk);
    bool success = (res == VM_OK);

    if (success && out_result) {
        *out_result = vm->last_result;
    } else if (!success) {
        fprintf(stderr, "[VM RUNTIME TRAP] %s\n", vm_result_str(res));
    }

    /* Step 4: Strict memory cleanup */
    ast_free(root);
    chunk_free(&chunk);
    vm_free(vm);

    return success;
}

/* ============================================================================
 * 8. MAIN ENTRYPOINT: CLI DRIVER & VERIFICATION TEST MATRIX
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("======================================================================\n");
    printf(" DAY 49: RECURSIVE-DESCENT AST PARSER & BYTECODE VIRTUAL MACHINE      \n");
    printf("======================================================================\n");

    /* CLI Mode: user passed arithmetic expression via argv */
    if (argc > 1) {
        char input_buffer[1024] = {0};
        for (int i = 1; i < argc; i++) {
            strncat(input_buffer, argv[i], sizeof(input_buffer) - strlen(input_buffer) - 2);
            if (i < argc - 1) strncat(input_buffer, " ", sizeof(input_buffer) - strlen(input_buffer) - 2);
        }

        Value result = 0.0;
        if (evaluate_expression(input_buffer, &result, true)) {
            printf("\n>>> Final Evaluated Result: %g <<<\n", result);
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    }

    /* Fallback Automated Verification Suite */
    printf("[No CLI args provided. Running Automated Parser & VM Test Suite...]\n\n");

    struct {
        const char *expression;
        Value       expected;
        bool        expect_fail;
    } test_matrix[] = {
        { "42",                                42.0,  false },
        { "10 + 20 * 3",                       70.0,  false },
        { "(10 + 20) * 3",                     90.0,  false },
        { "100 - 50 / 2 + 15 * 4",            135.0,  false },
        { "-12.5 * (4 + -2) / 5",              -5.0,  false },
        { "((2 + 3) * (7 - 2)) / (10 - 5)",     5.0,  false },
        { "10 / 0",                             0.0,  true  }, /* Division by zero trap */
        { "10 + * 5",                           0.0,  true  }  /* Syntax error recovery */
    };

    size_t count = sizeof(test_matrix) / sizeof(test_matrix[0]);
    int passed = 0;

    for (size_t i = 0; i < count; i++) {
        printf("Test [%zu/%zu]: \"%s\" ... ", i + 1, count, test_matrix[i].expression);
        Value res = 0.0;
        bool ok = evaluate_expression(test_matrix[i].expression, &res, false);

        if (ok && !test_matrix[i].expect_fail) {
            if (fabs(res - test_matrix[i].expected) < 1e-6) {
                printf("PASSED (Result: %g)\n", res);
                passed++;
            } else {
                printf("FAILED (Expected %g, Got %g)\n", test_matrix[i].expected, res);
            }
        } else if (!ok && test_matrix[i].expect_fail) {
            printf("PASSED (Expected Trap Triggered Cleanly)\n");
            passed++;
        } else {
            printf("FAILED (Unexpected Outcome)\n");
        }
    }

    printf("\n======================================================================\n");
    printf("Verification Summary: %d / %zu Test Cases Passed Cleanly.\n", passed, count);
    printf("======================================================================\n");

    return (passed == (int)count) ? EXIT_SUCCESS : EXIT_FAILURE;
}
