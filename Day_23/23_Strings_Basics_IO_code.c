/**
 * ============================================================================
 * Day 23: Strings Basics and Safe I/O
 * File: 23_Strings_Basics_IO_code.c
 * 
 * Overview:
 * In-depth demonstration of C string memory architecture, null terminator
 * mechanics ('\0'), difference between stack arrays and string literals,
 * safe reading via bounded buffers, newline stripping, and character inspection.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 23_Strings_Basics_IO_code.c -o 23_Strings_Basics_IO_code
 * 
 * Complexity:
 * - String traversal: O(N) where N is length before '\0'
 * - Space Complexity: O(N + 1) bytes
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 64

/**
 * Helper 1: Visualizes string memory byte-by-byte including ASCII code and '\0'
 */
void inspect_string_bytes(const char *label, const char *str, size_t max_bytes) {
    printf("[+] Memory Inspection for '%s':\n", label);
    printf("    Index | Char | ASCII (Dec) | Address\n");
    printf("    -------------------------------------------\n");
    for (size_t i = 0; i < max_bytes; ++i) {
        char ch = str[i];
        if (ch == '\0') {
            printf("    [%2zu]  | '\\0' |     %3d     | %p (NULL TERMINATOR)\n", i, (int)ch, (const void*)&str[i]);
            break; // Stop after inspecting the null byte
        } else if (ch >= 32 && ch <= 126) {
            printf("    [%2zu]  |  '%c' |     %3d     | %p\n", i, ch, (int)ch, (const void*)&str[i]);
        } else {
            printf("    [%2zu]  |  '?' |     %3d     | %p (Non-printable)\n", i, (int)ch, (const void*)&str[i]);
        }
    }
    printf("    -------------------------------------------\n\n");
}

/**
 * Helper 2: Strips trailing newline (\n or \r\n) from a buffer populated by fgets
 * Returns true if newline was found and removed, false otherwise.
 */
bool safe_strip_newline(char *buffer) {
    if (!buffer) return false;
    size_t len = strlen(buffer);
    if (len == 0) return false;

    if (buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        if (len > 1 && buffer[len - 2] == '\r') {
            buffer[len - 2] = '\0';
        }
        return true;
    }
    return false;
}

/**
 * Helper 3: Counts vowels, consonants, digits, and spaces in a string safely
 */
void analyze_string_characters(const char *str, int *vowels, int *consonants, int *digits, int *spaces) {
    *vowels = *consonants = *digits = *spaces = 0;
    if (!str) return;

    for (size_t i = 0; str[i] != '\0'; ++i) {
        char c = str[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            char lower = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
            if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
                (*vowels)++;
            } else {
                (*consonants)++;
            }
        } else if (c >= '0' && c <= '9') {
            (*digits)++;
        } else if (c == ' ' || c == '\t') {
            (*spaces)++;
        }
    }
}

int main(void) {
    printf("============================================================\n");
    printf("       DAY 23: STRINGS BASICS, MEMORY & SAFE I/O IN C       \n");
    printf("============================================================\n\n");

    // 1. Stack Array String vs Read-Only Literal
    char stack_str[16] = "C Programming";
    const char *literal_ptr = "Immutable";

    printf("[+] String Representation:\n");
    printf("    Stack array string:   \"%s\" (Length: %zu, Size: %zu)\n", 
           stack_str, strlen(stack_str), sizeof(stack_str));
    printf("    String literal ptr:   \"%s\" (Length: %zu, Ptr Size: %zu)\n\n", 
           literal_ptr, strlen(literal_ptr), sizeof(literal_ptr));

    // 2. Deep byte inspection
    inspect_string_bytes("Stack Array", stack_str, sizeof(stack_str));

    // 3. Simulating Safe Input and Processing
    char input_mock[BUFFER_SIZE] = "Systems Engineering in C99\n";
    printf("[+] Simulating safe input with trailing newline:\n");
    printf("    Raw mock buffer: \"%s\"\n", input_mock);
    
    bool stripped = safe_strip_newline(input_mock);
    printf("    Newline removed: %s\n", stripped ? "true" : "false");
    printf("    Sanitized buffer: \"%s\" (Length: %zu)\n\n", input_mock, strlen(input_mock));

    // 4. Character Composition Analysis
    int v, c, d, s;
    analyze_string_characters(input_mock, &v, &c, &d, &s);
    printf("[+] Character Analysis of \"%s\":\n", input_mock);
    printf("    Vowels:      %d\n", v);
    printf("    Consonants:  %d\n", c);
    printf("    Digits:      %d\n", d);
    printf("    Spaces:      %d\n", s);

    // 5. Modifying stack string in-place
    stack_str[0] = 'c'; // Valid for stack buffer
    printf("\n[+] Modified stack string in-place: \"%s\"\n", stack_str);

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}
