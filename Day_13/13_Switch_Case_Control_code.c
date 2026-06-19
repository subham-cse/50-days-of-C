/**
 * ============================================================================
 * Day 13: Switch-Case Control Structures & Jump Tables
 * Description : Production demonstration of switch-case dispatch, state machine
 *               execution, intentional fallthrough, and opcode decoding.
 * Compilation : gcc -Wall -Wextra -std=c99 13_Switch_Case_Control_code.c -o 13_Switch_Case_Control_code
 * Execution   : ./13_Switch_Case_Control_code
 * 
 * Complexity  : Time: O(1) for jump-table dispatchable switches
 *               Space: O(1) Stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>

/* Enumeration of a Finite State Machine (FSM) for a Network Connection */
typedef enum {
    CONN_DISCONNECTED = 0,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_TRANSFERRING,
    CONN_DISCONNECTING,
    CONN_ERROR_STATE
} ConnectionState;

typedef enum {
    EV_CONNECT_REQUEST,
    EV_HANDSHAKE_ACK,
    EV_SEND_PACKET,
    EV_TRANSFER_COMPLETE,
    EV_CLOSE_REQUEST,
    EV_TIMEOUT_ERROR
} ConnectionEvent;

/**
 * @brief Advances connection state based on current state and incoming event.
 * @param current Current state of connection.
 * @param event Triggering event.
 * @return Next state of connection.
 */
ConnectionState transition_connection_fsm(ConnectionState current, ConnectionEvent event) {
    ConnectionState next = current;

    switch (current) {
        case CONN_DISCONNECTED:
            switch (event) {
                case EV_CONNECT_REQUEST:
                    printf(" [FSM] Initiating TCP Handshake -> CONNECTING\n");
                    next = CONN_CONNECTING;
                    break;
                default:
                    printf(" [FSM Warning] Event ignored in DISCONNECTED state\n");
                    break;
            }
            break;

        case CONN_CONNECTING:
            switch (event) {
                case EV_HANDSHAKE_ACK:
                    printf(" [FSM] Handshake Acknowledged -> CONNECTED\n");
                    next = CONN_CONNECTED;
                    break;
                case EV_TIMEOUT_ERROR:
                    printf(" [FSM Error] Handshake Timed Out -> ERROR_STATE\n");
                    next = CONN_ERROR_STATE;
                    break;
                default:
                    break;
            }
            break;

        case CONN_CONNECTED:
            switch (event) {
                case EV_SEND_PACKET:
                    printf(" [FSM] Sending Stream -> TRANSFERRING\n");
                    next = CONN_TRANSFERRING;
                    break;
                case EV_CLOSE_REQUEST:
                    printf(" [FSM] Closing Connection -> DISCONNECTING\n");
                    next = CONN_DISCONNECTING;
                    break;
                default:
                    break;
            }
            break;

        case CONN_TRANSFERRING:
            switch (event) {
                case EV_TRANSFER_COMPLETE:
                    printf(" [FSM] Payload Finished -> CONNECTED\n");
                    next = CONN_CONNECTED;
                    break;
                case EV_TIMEOUT_ERROR:
                    printf(" [FSM Error] Socket dropped during transfer -> ERROR_STATE\n");
                    next = CONN_ERROR_STATE;
                    break;
                default:
                    break;
            }
            break;

        case CONN_DISCONNECTING:
            printf(" [FSM] Socket closed cleanly -> DISCONNECTED\n");
            next = CONN_DISCONNECTED;
            break;

        case CONN_ERROR_STATE:
            printf(" [FSM] Resetting from Error -> DISCONNECTED\n");
            next = CONN_DISCONNECTED;
            break;

        default:
            printf(" [FSM Bug] Unrecognized state encounter!\n");
            next = CONN_DISCONNECTED;
            break;
    }

    return next;
}

/**
 * @brief Evaluates days in a month using intentional switch-case fallthrough.
 * @param month 1 to 12
 * @param is_leap_year true if leap year
 * @return Number of days in month, or -1 if invalid
 */
int get_days_in_month(int month, bool is_leap_year) {
    switch (month) {
        /* Months with 31 days */
        case 1:  /* January */
        case 3:  /* March */
        case 5:  /* May */
        case 7:  /* July */
        case 8:  /* August */
        case 10: /* October */
        case 12: /* December */
            return 31;

        /* Months with 30 days */
        case 4:  /* April */
        case 6:  /* June */
        case 9:  /* September */
        case 11: /* November */
            return 30;

        /* February */
        case 2:
            return is_leap_year ? 29 : 28;

        default:
            return -1; /* Invalid month */
    }
}

int main(void) {
    printf("===================================================================\n");
    printf("         DAY 13: SWITCH-CASE CONTROL & STATE MACHINES              \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Intentional Fallthrough for Calendar Days */
    printf("[TEST SUITE 1: Calendar Days Calculation via Fallthrough]\n");
    for (int m = 1; m <= 12; ++m) {
        bool leap = (m == 2); /* Test leap year for Feb */
        int days = get_days_in_month(m, leap);
        printf("Month %2d: %d days %s\n", m, days, (m == 2) ? "(Leap Year Check)" : "");
    }

    /* Test Suite 2: Protocol State Machine Execution */
    printf("\n[TEST SUITE 2: Network Protocol State Machine Walkthrough]\n");
    ConnectionState state = CONN_DISCONNECTED;

    ConnectionEvent event_sequence[] = {
        EV_CONNECT_REQUEST,
        EV_HANDSHAKE_ACK,
        EV_SEND_PACKET,
        EV_TRANSFER_COMPLETE,
        EV_CLOSE_REQUEST
    };
    size_t seq_len = sizeof(event_sequence) / sizeof(event_sequence[0]);

    for (size_t i = 0; i < seq_len; ++i) {
        printf("Step %zu | Current State: %d -> Processing Event: %d\n", i + 1, state, event_sequence[i]);
        state = transition_connection_fsm(state, event_sequence[i]);
    }

    printf("\nFinal Connection State: %d (0 = DISCONNECTED)\n", state);
    printf("===================================================================\n");
    return 0;
}
