/**
 * ============================================================================
 * 50 Days of C - Domain Engine: Recursive-Descent Expression Parser
 * File: engine/parser.c
 * Standard: ISO C99 / C11
 * ============================================================================
 * Description:
 *   A recursive-descent parser and lexical scanner for mathematical expressions.
 *   Directly compiles arithmetic syntax trees into stack-based bytecode instructions
 *   for the custom Bytecode Virtual Machine engine (engine/vm.c).
 *
 * Grammar:
 *   Expression ::= Term ( ( '+' | '-' ) Term )*
 *   Term       ::= Factor ( ( '*' | '/' ) Factor )*
 *   Factor     ::= ( '+' | '-' ) Factor | Primary
 *   Primary    ::= NUMBER | '(' Expression ')'
 * ============================================================================
 */

#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef _WIN32
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

/* ============================================================================
 * 1. LEXICAL TOKEN DEFINITIONS & SCANNER
 * ============================================================================
 */

typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType   type;
    const char *start;
    size_t      length;
    int         line;
    Value       value;
} Token;

typedef struct {
    const char *source;
    const char *current;
    int         line;
} Lexer;

static Lexer g_lexer;

static void lexer_init(const char *source)
{
    g_lexer.source = source;
    g_lexer.current = source;
    g_lexer.line = 1;
}

static bool is_at_end(void)
{
    return *g_lexer.current == '\0';
}

static char advance_char(void)
{
    g_lexer.current++;
    return g_lexer.current[-1];
}

static char peek_char(void)
{
    return *g_lexer.current;
}

static void skip_whitespace(void)
{
    for (;;) {
        char c = peek_char();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance_char();
                break;
            case '\n':
                g_lexer.line++;
                advance_char();
                break;
            default:
                return;
        }
    }
}

static Token make_token(TokenType type)
{
    Token token;
    token.type = type;
    token.start = g_lexer.current - 1;
    token.length = 1;
    token.line = g_lexer.line;
    token.value = 0.0;
    return token;
}

static Token make_error_token(const char *message)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = strlen(message);
    token.line = g_lexer.line;
    token.value = 0.0;
    return token;
}

static Token scan_number(void)
{
    const char *start = g_lexer.current - 1;
    while (isdigit((unsigned char)peek_char())) {
        advance_char();
    }

    /* Check for fractional floating point part */
    if (peek_char() == '.' && isdigit((unsigned char)g_lexer.current[1])) {
        advance_char(); /* Consume '.' */
        while (isdigit((unsigned char)peek_char())) {
            advance_char();
        }
    }

    char *end_ptr = NULL;
    Value val = strtod(start, &end_ptr);

    Token token;
    token.type = TOKEN_NUMBER;
    token.start = start;
    token.length = (size_t)(g_lexer.current - start);
    token.line = g_lexer.line;
    token.value = val;
    return token;
}

static Token scan_token(void)
{
    skip_whitespace();

    if (is_at_end()) {
        Token eof_token;
        eof_token.type = TOKEN_EOF;
        eof_token.start = g_lexer.current;
        eof_token.length = 0;
        eof_token.line = g_lexer.line;
        eof_token.value = 0.0;
        return eof_token;
    }

    char c = advance_char();

    if (isdigit((unsigned char)c) || (c == '.' && isdigit((unsigned char)peek_char()))) {
        return scan_number();
    }

    switch (c) {
        case '+': return make_token(TOKEN_PLUS);
        case '-': return make_token(TOKEN_MINUS);
        case '*': return make_token(TOKEN_STAR);
        case '/': return make_token(TOKEN_SLASH);
        case '(': return make_token(TOKEN_LPAREN);
        case ')': return make_token(TOKEN_RPAREN);
        default:
            return make_error_token("Unexpected character in expression.");
    }
}

/* ============================================================================
 * 2. RECURSIVE-DESCENT PARSER & BYTECODE EMITTER
 * ============================================================================
 */

typedef struct {
    Token current;
    Token previous;
    bool  had_error;
    bool  panic_mode;
    Chunk *chunk;
} Parser;

static Parser g_parser;

static void parser_error_at(Token *token, const char *message)
{
    if (g_parser.panic_mode) return;
    g_parser.panic_mode = true;
    g_parser.had_error = true;

    fprintf(stderr, "[PARSER ERROR] line %d", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end of expression");
    } else if (token->type == TOKEN_ERROR) {
        /* Generic error token */
    } else {
        fprintf(stderr, " at '%.*s'", (int)token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
}

static void advance_token(void)
{
    g_parser.previous = g_parser.current;

    for (;;) {
        g_parser.current = scan_token();
        if (g_parser.current.type != TOKEN_ERROR) break;
        parser_error_at(&g_parser.current, g_parser.current.start);
    }
}

static bool check_token(TokenType type)
{
    return g_parser.current.type == type;
}

static bool match_token(TokenType type)
{
    if (!check_token(type)) return false;
    advance_token();
    return true;
}

static void consume_token(TokenType type, const char *message)
{
    if (g_parser.current.type == type) {
        advance_token();
        return;
    }
    parser_error_at(&g_parser.current, message);
}

static void emit_byte(uint8_t byte)
{
    chunk_write_byte(g_parser.chunk, byte);
}

static void emit_constant(Value value)
{
    size_t const_idx = chunk_add_constant(g_parser.chunk, value);
    if (const_idx > 65535) {
        parser_error_at(&g_parser.previous, "Too many constants in single expression.");
        return;
    }
    emit_byte(OP_PUSH);
    chunk_write_uint16(g_parser.chunk, (uint16_t)const_idx);
}

/* Forward declarations of recursive grammar rules */
static void parse_expression(void);
static void parse_term(void);
static void parse_factor(void);
static void parse_primary(void);

static void parse_primary(void)
{
    if (match_token(TOKEN_NUMBER)) {
        emit_constant(g_parser.previous.value);
    } else if (match_token(TOKEN_LPAREN)) {
        parse_expression();
        consume_token(TOKEN_RPAREN, "Expected ')' after sub-expression.");
    } else {
        parser_error_at(&g_parser.current, "Expected numeric literal or '(' sub-expression.");
    }
}

static void parse_factor(void)
{
    if (match_token(TOKEN_MINUS)) {
        /* Unary negation: compile operand then emit OP_NEG */
        parse_factor();
        emit_byte(OP_NEG);
    } else if (match_token(TOKEN_PLUS)) {
        /* Unary positive sign: no-op in stack arithmetic */
        parse_factor();
    } else {
        parse_primary();
    }
}

static void parse_term(void)
{
    parse_factor();

    while (match_token(TOKEN_STAR) || match_token(TOKEN_SLASH)) {
        TokenType op = g_parser.previous.type;
        parse_factor();

        if (op == TOKEN_STAR) {
            emit_byte(OP_MUL);
        } else {
            emit_byte(OP_DIV);
        }
    }
}

static void parse_expression(void)
{
    parse_term();

    while (match_token(TOKEN_PLUS) || match_token(TOKEN_MINUS)) {
        TokenType op = g_parser.previous.type;
        parse_term();

        if (op == TOKEN_PLUS) {
            emit_byte(OP_ADD);
        } else {
            emit_byte(OP_SUB);
        }
    }
}

/**
 * Compiles mathematical source string directly into VM bytecode chunk.
 * Returns true if compilation succeeded with 0 syntax errors.
 */
bool parser_compile(const char *source, Chunk *chunk)
{
    if (!source || !chunk) return false;

    lexer_init(source);
    g_parser.had_error = false;
    g_parser.panic_mode = false;
    g_parser.chunk = chunk;

    advance_token();
    parse_expression();
    consume_token(TOKEN_EOF, "Expected end of expression.");

    /* Emit output and halt directives */
    emit_byte(OP_PRINT);
    emit_byte(OP_HALT);

    return !g_parser.had_error;
}

/* ============================================================================
 * 3. EXECUTION DRIVER & DEMONSTRATION HARNESS
 * ============================================================================
 */

typedef struct {
    const char *expression;
    double      expected;
    bool        should_fail;
} TestCase;

static void run_eval_pipeline(const char *expr_str, bool verbose)
{
    printf("\n======================================================================\n");
    printf("Evaluating Expression: \"%s\"\n", expr_str);
    printf("======================================================================\n");

    Chunk chunk;
    chunk_init(&chunk);

    if (!parser_compile(expr_str, &chunk)) {
        fprintf(stderr, "[PIPELINE FAILURE] Syntax compilation failed.\n");
        chunk_free(&chunk);
        return;
    }

    if (verbose) {
        chunk_disassemble(&chunk, expr_str);
    }

    VM *vm = vm_create(256);
    if (!vm) {
        fprintf(stderr, "[FATAL] Failed to initialize Virtual Machine.\n");
        chunk_free(&chunk);
        return;
    }

    VMResult res = vm_run(vm, &chunk);
    if (res != VM_OK) {
        fprintf(stderr, "[RUNTIME ERROR] %s\n", vm_result_to_string(res));
    } else {
        printf("[SUCCESS] Evaluated Output: %g\n", vm->last_result);
    }

    vm_free(vm);
    chunk_free(&chunk);
}

int main(int argc, char *argv[])
{
    printf("**********************************************************************\n");
    printf("*  50-Days-of-C Domain Engine: Recursive Descent Parser & VM Engine  *\n");
    printf("**********************************************************************\n");

    if (argc > 1) {
        /* Command-line expression evaluation mode */
        char buffer[1024] = {0};
        for (int i = 1; i < argc; i++) {
            strncat(buffer, argv[i], sizeof(buffer) - strlen(buffer) - 2);
            if (i < argc - 1) strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 2);
        }
        run_eval_pipeline(buffer, true);
        return 0;
    }

    /* Automated Test & Verification Matrix */
    printf("\nRunning Architectural Domain Engine Self-Tests...\n");

    TestCase tests[] = {
        {"42", 42.0, false},
        {"10 + 20 * 3", 70.0, false},
        {"(10 + 20) * 3", 90.0, false},
        {"100 - 50 / 2 + 15 * 4", 135.0, false},
        {"-12.5 * (4 + -2) / 5", -5.0, false},
        {"((2 + 3) * (7 - 2)) / (10 - 5)", 5.0, false},
        {"10 / 0", 0.0, true},          /* Division by zero runtime safety check */
        {"10 + * 5", 0.0, true}         /* Syntax error recovery check */
    };

    size_t num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (size_t i = 0; i < num_tests; i++) {
        printf("\n[Self-Test %zu/%zu] Expression: \"%s\"\n", i + 1, num_tests, tests[i].expression);

        Chunk chunk;
        chunk_init(&chunk);

        bool compiled = parser_compile(tests[i].expression, &chunk);
        if (!compiled) {
            if (tests[i].should_fail) {
                printf("  -> Expected syntax rejection: PASSED\n");
                passed++;
            } else {
                printf("  -> Unexpected compilation failure: FAILED\n");
            }
            chunk_free(&chunk);
            continue;
        }

        VM *vm = vm_create(128);
        VMResult res = vm_run(vm, &chunk);

        if (res != VM_OK) {
            if (tests[i].should_fail) {
                printf("  -> Expected runtime trap (%s): PASSED\n", vm_result_to_string(res));
                passed++;
            } else {
                printf("  -> Unexpected runtime error (%s): FAILED\n", vm_result_to_string(res));
            }
        } else {
            if (!tests[i].should_fail) {
                printf("  -> Result = %g (Expected: %g): PASSED\n", vm->last_result, tests[i].expected);
                passed++;
            } else {
                printf("  -> Expected failure but got result: %g: FAILED\n", vm->last_result);
            }
        }

        vm_free(vm);
        chunk_free(&chunk);
    }

    printf("\n----------------------------------------------------------------------\n");
    printf("Domain Engine Verification Summary: %d / %zu Test Cases Passed.\n", passed, num_tests);
    printf("----------------------------------------------------------------------\n");

    return (passed == (int)num_tests) ? 0 : 1;
}
