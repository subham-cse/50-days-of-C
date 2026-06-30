/**
 * ============================================================================
 * Day 24: Standard String Functions & Custom Implementations
 * File: 24_Strlen_Strcpy_Strcat_Strcmp_code.c
 * 
 * Overview:
 * Clean, safe, production-grade custom implementations of the classic C string
 * functions (strlen, strcpy, strcat, strcmp), along with comparison against
 * standard library equivalents and modern bounds-checked snprintf patterns.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 24_Strlen_Strcpy_Strcat_Strcmp_code.c -o 24_Strlen_Strcpy_Strcat_Strcmp_code
 * 
 * Complexity:
 * - custom_strlen: O(N) time, O(1) space
 * - custom_strcpy: O(N) time, O(1) space
 * - custom_strcat: O(M + N) time, O(1) space
 * - custom_strcmp: O(min(M, N)) time, O(1) space
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/**
 * Helper 1: Custom strlen implementation using pointer difference
 */
size_t custom_strlen(const char *str) {
    if (!str) return 0;
    const char *ptr = str;
    while (*ptr != '\0') {
        ptr++;
    }
    return (size_t)(ptr - str);
}

/**
 * Helper 2: Custom bounded string copy (similar to safe strlcpy semantics)
 * Returns the total length of the source string it tried to create.
 */
size_t custom_strcpy_safe(char *dest, size_t dest_size, const char *src) {
    if (!dest || dest_size == 0) return 0;
    if (!src) {
        dest[0] = '\0';
        return 0;
    }

    size_t i = 0;
    while (i + 1 < dest_size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // Always guarantee null termination

    // Compute total source length for truncation detection
    return custom_strlen(src);
}

/**
 * Helper 3: Custom bounded string concatenation
 */
bool custom_strcat_safe(char *dest, size_t dest_size, const char *src) {
    if (!dest || !src || dest_size == 0) return false;

    size_t dest_len = custom_strlen(dest);
    if (dest_len >= dest_size - 1) return false; // No room to append

    size_t space_remaining = dest_size - dest_len - 1;
    size_t i = 0;
    while (i < space_remaining && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
    return true;
}

/**
 * Helper 4: Custom strcmp implementation casting to unsigned char
 */
int custom_strcmp(const char *s1, const char *s2) {
    assert(s1 != NULL && s2 != NULL);
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (int)((unsigned char)*s1 - (unsigned char)*s2);
}

int main(void) {
    printf("============================================================\n");
    printf("     DAY 24: STRING MANIPULATION & SAFE BOUNDED UTILS       \n");
    printf("============================================================\n\n");

    const char *sample = "Systems Architecture in C";

    // 1. Length verification
    size_t len_std = strlen(sample);
    size_t len_custom = custom_strlen(sample);
    printf("[+] String Length Verification:\n");
    printf("    String:         \"%s\"\n", sample);
    printf("    Standard strlen: %zu\n", len_std);
    printf("    Custom strlen:   %zu\n", len_custom);
    assert(len_std == len_custom);
    printf("    Status: [PASS]\n\n");

    // 2. Safe Copy with truncation demonstration
    char buffer[16];
    size_t src_len = custom_strcpy_safe(buffer, sizeof(buffer), sample);
    printf("[+] Safe Bounded Copy:\n");
    printf("    Buffer capacity: %zu bytes\n", sizeof(buffer));
    printf("    Copied string:   \"%s\"\n", buffer);
    printf("    Source length:   %zu\n", src_len);
    if (src_len >= sizeof(buffer)) {
        printf("    Truncation Notice: Source was safely truncated to prevent buffer overflow.\n\n");
    }

    // 3. Safe Concatenation
    char combined[32] = "Hello";
    printf("[+] Safe Concatenation:\n");
    printf("    Original string: \"%s\"\n", combined);
    custom_strcat_safe(combined, sizeof(combined), ", World of C!");
    printf("    Appended string: \"%s\"\n\n", combined);

    // 4. Lexicographical Comparison Testing
    const char *strA = "Apple";
    const char *strB = "Banana";
    const char *strC = "Apple";

    printf("[+] String Comparison Analysis:\n");
    int cmp_AB = custom_strcmp(strA, strB);
    int cmp_BA = custom_strcmp(strB, strA);
    int cmp_AA = custom_strcmp(strA, strC);

    printf("    strcmp(\"%s\", \"%s\") = %d (A < B)\n", strA, strB, cmp_AB);
    printf("    strcmp(\"%s\", \"%s\") = %d (B > A)\n", strB, strA, cmp_BA);
    printf("    strcmp(\"%s\", \"%s\") = %d (Equal)\n", strA, strC, cmp_AA);

    assert(cmp_AB < 0 && cmp_BA > 0 && cmp_AA == 0);
    printf("    Status: [PASS] All comparisons lexicographically accurate.\n");

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}
