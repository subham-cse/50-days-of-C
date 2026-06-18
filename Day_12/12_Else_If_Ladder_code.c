/**
 * ============================================================================
 * Day 12: Else-If Ladder & Multi-Way Selection
 * Description : Production implementation of cascaded conditionals for HTTP
 *               status code classification and income tax progressive brackets.
 * Compilation : gcc -Wall -Wextra -std=c99 12_Else_If_Ladder_code.c -o 12_Else_If_Ladder_code
 * Execution   : ./12_Else_If_Ladder_code
 * 
 * Complexity  : Time: O(K) where K is number of conditions evaluated
 *               Space: O(1) Stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Classifies an HTTP status code into its RFC architectural category.
 * @param code HTTP status code integer (e.g., 200, 404, 500).
 * @return String description of the HTTP response category.
 */
const char* classify_http_status(int code) {
    if (code >= 100 && code <= 199) {
        return "1xx: Informational (Request received, continuing process)";
    } else if (code >= 200 && code <= 299) {
        return "2xx: Success (Action successfully received, understood, and accepted)";
    } else if (code >= 300 && code <= 399) {
        return "3xx: Redirection (Further action needed to complete request)";
    } else if (code >= 400 && code <= 499) {
        return "4xx: Client Error (Request contains bad syntax or cannot be fulfilled)";
    } else if (code >= 500 && code <= 599) {
        return "5xx: Server Error (Server failed to fulfill an apparently valid request)";
    } else {
        return "Unknown / Invalid HTTP Status Code";
    }
}

/**
 * @brief Computes progressive marginal tax based on income tiers using an else-if ladder.
 * @param taxable_income Total taxable income in currency units.
 * @return Total computed tax obligation.
 */
double calculate_progressive_income_tax(double taxable_income) {
    if (taxable_income <= 0.0) {
        return 0.0;
    } else if (taxable_income <= 11000.0) {
        /* Tier 1: 10% on income up to $11,000 */
        return taxable_income * 0.10;
    } else if (taxable_income <= 44725.0) {
        /* Tier 2: $1,100 + 12% on income between $11,000 and $44,725 */
        return 1100.0 + (taxable_income - 11000.0) * 0.12;
    } else if (taxable_income <= 95375.0) {
        /* Tier 3: $5,147 + 22% on income between $44,725 and $95,375 */
        return 5147.0 + (taxable_income - 44725.0) * 0.22;
    } else if (taxable_income <= 182100.0) {
        /* Tier 4: $16,290 + 24% on income between $95,375 and $182,100 */
        return 16290.0 + (taxable_income - 95375.0) * 0.24;
    } else {
        /* Tier 5: $37,104 + 32% on income exceeding $182,100 */
        return 37104.0 + (taxable_income - 182100.0) * 0.32;
    }
}

int main(void) {
    printf("===================================================================\n");
    printf("           DAY 12: ELSE-IF LADDER MULTI-WAY CLASSIFICATION         \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: HTTP Status Code Parser */
    printf("[TEST SUITE 1: HTTP Status Codes Classifier]\n");
    int test_codes[] = {101, 200, 204, 301, 403, 404, 500, 503, 999};
    size_t num_codes = sizeof(test_codes) / sizeof(test_codes[0]);

    for (size_t i = 0; i < num_codes; ++i) {
        printf("Status %-3d -> %s\n", test_codes[i], classify_http_status(test_codes[i]));
    }

    /* Test Suite 2: Progressive Tax Calculation */
    printf("\n[TEST SUITE 2: Progressive Marginal Income Tax Calculator]\n");
    double income_samples[] = {0.0, 9500.0, 35000.0, 75000.0, 140000.0, 250000.0};
    size_t num_samples = sizeof(income_samples) / sizeof(income_samples[0]);

    for (size_t j = 0; j < num_samples; ++j) {
        double income = income_samples[j];
        double tax = calculate_progressive_income_tax(income);
        double effective_rate = (income > 0.0) ? (tax / income) * 100.0 : 0.0;

        printf("Income: $%10.2f | Tax: $%9.2f | Effective Rate: %5.2f%%\n",
               income, tax, effective_rate);
    }

    printf("\n===================================================================\n");
    printf("All multi-way branch ladders executed successfully.\n");
    return 0;
}
