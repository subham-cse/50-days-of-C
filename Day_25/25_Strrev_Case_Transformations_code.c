/**
 * ============================================================================
 * Day 25: String Reversal and Case Transformations
 * File: 25_Strrev_Case_Transformations_code.c
 * 
 * Overview:
 * Comprehensive implementation of string manipulation algorithms:
 * 1. In-place string reversal using a two-pointer technique.
 * 2. Case conversions (to uppercase, to lowercase, title case, toggle case).
 * 3. ASCII bitwise case manipulation.
 * 4. Robust alphanumeric palindrome validation.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 25_Strrev_Case_Transformations_code.c -o 25_Strrev_Case_Transformations_code
 * 
 * Complexity:
 * - Time: O(N) single-pass or two-pass
 * - Space: O(1) in-place auxiliary memory
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/**
 * Helper 1: In-Place String Reversal using Two-Pointer Technique
 */
void reverse_string_inplace(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    if (len <= 1) return;

    size_t left = 0;
    size_t right = len - 1;

    while (left < right) {
        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
        right--;
    }
}

/**
 * Helper 2: Case Transformations
 */
void to_upper_inplace(char *str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

void to_lower_inplace(char *str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

void toggle_case_bitwise(char *str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z')) {
            str[i] ^= 0x20; // Flip 5th bit (32) to swap ASCII case
        }
    }
}

/**
 * Helper 3: Title Case Converter
 * Capitalizes the first character of each word and lowercases all subsequent characters.
 */
void to_title_case(char *str) {
    if (!str) return;
    bool new_word = true;

    for (size_t i = 0; str[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)str[i];
        if (isspace(c)) {
            new_word = true;
        } else if (new_word) {
            str[i] = (char)toupper(c);
            new_word = false;
        } else {
            str[i] = (char)tolower(c);
        }
    }
}

/**
 * Helper 4: Robust Alphanumeric Palindrome Validator (Ignores punctuation and case)
 */
bool is_palindrome_sentence(const char *str) {
    if (!str) return false;
    size_t len = strlen(str);
    if (len <= 1) return true;

    size_t left = 0;
    size_t right = len - 1;

    while (left < right) {
        // Skip non-alphanumeric characters on left
        while (left < right && !isalnum((unsigned char)str[left])) {
            left++;
        }
        // Skip non-alphanumeric characters on right
        while (left < right && !isalnum((unsigned char)str[right])) {
            right--;
        }

        if (tolower((unsigned char)str[left]) != tolower((unsigned char)str[right])) {
            return false;
        }

        left++;
        if (right > 0) right--;
    }
    return true;
}

int main(void) {
    printf("============================================================\n");
    printf("     DAY 25: STRING REVERSAL & CASE TRANSFORMATIONS IN C    \n");
    printf("============================================================\n\n");

    // 1. In-Place Reversal Demonstration
    char text1[64] = "Kernel Architecture";
    printf("[+] In-Place Reversal:\n");
    printf("    Original:  \"%s\"\n", text1);
    reverse_string_inplace(text1);
    printf("    Reversed:  \"%s\"\n\n", text1);

    // 2. Case Conversions
    char text2[64] = "Low Level Embedded C";
    printf("[+] Case Conversions on \"%s\":\n", text2);
    
    to_upper_inplace(text2);
    printf("    Uppercase: \"%s\"\n", text2);
    
    to_lower_inplace(text2);
    printf("    Lowercase: \"%s\"\n", text2);

    to_title_case(text2);
    printf("    Title Case:\"%s\"\n", text2);

    // 3. Bitwise Toggle Case
    toggle_case_bitwise(text2);
    printf("    Bit Toggle:\"%s\"\n\n", text2);

    // 4. Palindrome Testing Suite
    const char *test_cases[] = {
        "racecar",
        "A man, a plan, a canal: Panama",
        "Was it a car or a cat I saw?",
        "Hello, World!",
        "12321",
        "123456"
    };
    size_t num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("[+] Palindrome Evaluation Suite:\n");
    for (size_t i = 0; i < num_tests; ++i) {
        bool result = is_palindrome_sentence(test_cases[i]);
        printf("    \"%-32s\" -> %s\n", test_cases[i], result ? "[PALINDROME]" : "[NOT PALINDROME]");
    }

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}
