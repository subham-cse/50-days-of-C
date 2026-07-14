/**
 * ============================================================================
 * Day 38: Enumerations (enum) & Type Aliasing (typedef)
 * File: 38_Enums_Typedef_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 38_Enums_Typedef_code.c -o 38_Enums_Typedef_code
 * 
 * Description:
 *   Demonstrates basic and custom-valued enums, bitmask flags with bitwise ops,
 *   Finite State Machine (FSM) implementation, typedef type safety vs macro traps,
 *   and function pointer typedef callbacks.
 * 
 * Complexity:
 *   Time Complexity:  O(1) state transitions and bit operations
 *   Space Complexity: O(1) stack allocation
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* 1. Basic Enumeration with custom values */
typedef enum {
    LOG_LEVEL_DEBUG = 10,
    LOG_LEVEL_INFO  = 20,
    LOG_LEVEL_WARN  = 30,
    LOG_LEVEL_ERROR = 40,
    LOG_LEVEL_FATAL = 50
} LogLevel;

/* 2. Bitmask Enumeration (Powers of 2 for flag registers) */
typedef enum {
    PERM_NONE    = 0,
    PERM_READ    = 1 << 0, /* 0x01 */
    PERM_WRITE   = 1 << 1, /* 0x02 */
    PERM_EXECUTE = 1 << 2, /* 0x04 */
    PERM_ADMIN   = 1 << 3  /* 0x08 */
} FilePermission;

/* 3. Finite State Machine (FSM) States and Events */
typedef enum {
    STATE_IDLE,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_ERROR
} ConnectionState;

typedef enum {
    EVENT_START_CONNECT,
    EVENT_CONNECT_SUCCESS,
    EVENT_CONNECT_FAIL,
    EVENT_DISCONNECT,
    EVENT_RESET
} ConnectionEvent;

/* 4. Function Pointer Typedef for arithmetic transformations */
typedef int32_t (*MathTransform)(int32_t input);

/* Typedef Pointer vs Macro demonstration aliases */
#define MACRO_INTPTR int*
typedef int* TypedefIntPtr;

/**
 * @brief Evaluates state transition in a network FSM.
 * 
 * @param current Current connection state.
 * @param evt Incoming event trigger.
 * @return Next state after transition.
 */
ConnectionState transition_fsm(ConnectionState current, ConnectionEvent evt) {
    switch (current) {
        case STATE_IDLE:
            if (evt == EVENT_START_CONNECT) return STATE_CONNECTING;
            break;
        case STATE_CONNECTING:
            if (evt == EVENT_CONNECT_SUCCESS) return STATE_CONNECTED;
            if (evt == EVENT_CONNECT_FAIL) return STATE_ERROR;
            break;
        case STATE_CONNECTED:
            if (evt == EVENT_DISCONNECT) return STATE_DISCONNECTED;
            break;
        case STATE_DISCONNECTED:
        case STATE_ERROR:
            if (evt == EVENT_RESET) return STATE_IDLE;
            break;
        default:
            break;
    }
    return current; /* Unchanged if transition not valid */
}

/**
 * @brief Returns human-readable string representation of FSM state.
 */
const char* state_to_string(ConnectionState state) {
    switch (state) {
        case STATE_IDLE:         return "IDLE";
        case STATE_CONNECTING:   return "CONNECTING";
        case STATE_CONNECTED:    return "CONNECTED";
        case STATE_DISCONNECTED: return "DISCONNECTED";
        case STATE_ERROR:        return "ERROR";
        default:                 return "UNKNOWN_STATE";
    }
}

/**
 * @brief Demonstrates bitmask operations with enum permissions.
 * 
 * @param perms Current permission bitmask.
 */
void print_permissions(uint32_t perms) {
    printf("  Current Bitmask: 0x%02X -> [ ", perms);
    if (perms & PERM_READ)    printf("READ ");
    if (perms & PERM_WRITE)   printf("WRITE ");
    if (perms & PERM_EXECUTE) printf("EXEC ");
    if (perms & PERM_ADMIN)   printf("ADMIN ");
    if (perms == PERM_NONE)   printf("NONE ");
    printf("]\n");
}

/* Sample Math Transformation Helpers */
static int32_t square_num(int32_t x) { return x * x; }
static int32_t cube_num(int32_t x)   { return x * x * x; }

/**
 * @brief Applies a function pointer callback transformation to an array.
 */
void apply_transform(const int32_t *src, int32_t *dest, size_t count, MathTransform transform) {
    for (size_t i = 0; i < count; ++i) {
        dest[i] = transform(src[i]);
    }
}

/**
 * @brief Highlights difference between #define pointer trap and typedef pointer.
 */
void demonstrate_typedef_vs_macro_trap(void) {
    printf("--- [Typedef vs #define Pointer Trap Demonstration] ---\n");
    
    int base_val = 42;
    int other_val = 100;

    /* 1. Macro Trap: MACRO_INTPTR a, b expands to int* a, b; (b is NOT a pointer!) */
    MACRO_INTPTR m_ptr, m_int;
    m_ptr = &base_val;
    m_int = other_val; /* Compiles because m_int is plain 'int', NOT 'int*' */

    /* 2. Typedef Safety: TypedefIntPtr c, d creates two true pointers */
    TypedefIntPtr t_ptr1, t_ptr2;
    t_ptr1 = &base_val;
    t_ptr2 = &other_val;

    printf("Macro result:   *m_ptr = %d, m_int = %d (Not a pointer!)\n", *m_ptr, m_int);
    printf("Typedef result: *t_ptr1 = %d, *t_ptr2 = %d (Both are valid pointers)\n\n", *t_ptr1, *t_ptr2);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 38: ENUMERATIONS (enum) & TYPE ALIASING (typedef)\n");
    printf("===============================================================\n\n");

    /* 1. Enum bitmask operations */
    printf("[1] Bitmask Flag Operations with Enums:\n");
    uint32_t user_perms = PERM_NONE;
    print_permissions(user_perms);

    /* Grant READ and WRITE */
    user_perms |= (PERM_READ | PERM_WRITE);
    printf("After granting READ | WRITE:\n");
    print_permissions(user_perms);

    /* Grant ADMIN */
    user_perms |= PERM_ADMIN;
    printf("After granting ADMIN:\n");
    print_permissions(user_perms);

    /* Revoke WRITE */
    user_perms &= ~PERM_WRITE;
    printf("After revoking WRITE (~PERM_WRITE):\n");
    print_permissions(user_perms);
    printf("\n");

    /* 2. Finite State Machine Transition Walkthrough */
    printf("[2] Finite State Machine Simulation:\n");
    ConnectionState current_state = STATE_IDLE;
    printf("Initial State: %s\n", state_to_string(current_state));

    current_state = transition_fsm(current_state, EVENT_START_CONNECT);
    printf("Trigger EVENT_START_CONNECT -> State: %s\n", state_to_string(current_state));

    current_state = transition_fsm(current_state, EVENT_CONNECT_SUCCESS);
    printf("Trigger EVENT_CONNECT_SUCCESS -> State: %s\n", state_to_string(current_state));

    current_state = transition_fsm(current_state, EVENT_DISCONNECT);
    printf("Trigger EVENT_DISCONNECT -> State: %s\n", state_to_string(current_state));

    current_state = transition_fsm(current_state, EVENT_RESET);
    printf("Trigger EVENT_RESET -> State: %s\n\n", state_to_string(current_state));

    /* 3. Function Pointer Typedef */
    printf("[3] Function Pointer Typedef Transformations:\n");
    int32_t numbers[4] = { 2, 4, 6, 8 };
    int32_t squared[4] = { 0 };
    int32_t cubed[4] = { 0 };

    apply_transform(numbers, squared, 4, square_num);
    apply_transform(numbers, cubed, 4, cube_num);

    printf("Original: [ ");
    for (int i = 0; i < 4; ++i) printf("%d ", numbers[i]);
    printf("]\nSquared:  [ ");
    for (int i = 0; i < 4; ++i) printf("%d ", squared[i]);
    printf("]\nCubed:    [ ");
    for (int i = 0; i < 4; ++i) printf("%d ", cubed[i]);
    printf("]\n\n");

    /* 4. Typedef vs Macro Trap */
    demonstrate_typedef_vs_macro_trap();

    return 0;
}
