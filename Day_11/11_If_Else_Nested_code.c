/**
 * ============================================================================
 * Day 11: Nested If-Else Statements & Hierarchical Branching
 * Description : Production-grade demonstration of nested conditional branches,
 *               decision trees, authentication guards, and validation workflows.
 * Compilation : gcc -Wall -Wextra -std=c99 11_If_Else_Nested_code.c -o 11_If_Else_Nested_code
 * Execution   : ./11_If_Else_Nested_code
 * 
 * Complexity  : Time: O(1) - Constant number of comparisons
 *               Space: O(1) - Stack allocation only
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>

/* Status codes representing system states */
typedef enum {
    AUTH_SUCCESS = 0,
    AUTH_ERR_USER_NOT_FOUND,
    AUTH_ERR_INVALID_PASSWORD,
    AUTH_ERR_ACCOUNT_LOCKED,
    AUTH_ERR_INSUFFICIENT_PERMISSIONS
} AuthResult;

typedef struct {
    int user_id;
    bool is_active;
    bool is_locked;
    int security_level; /* 1 = Basic, 2 = Operator, 3 = Administrator */
    int password_hash;  /* Mock hashed password representation */
} UserAccount;

/**
 * @brief Evaluates multi-tier access privileges using nested if-else statements.
 * @param account Pointer to constant UserAccount structure.
 * @param entered_hash Password hash input by user.
 * @param required_level Required minimum privilege level for target resource.
 * @return AuthResult status code.
 */
AuthResult evaluate_user_access_nested(const UserAccount *account, int entered_hash, int required_level) {
    /* Level 1: Account presence check */
    if (account != NULL) {
        /* Level 2: Active and lock status check */
        if (account->is_active) {
            if (!account->is_locked) {
                /* Level 3: Credential verification */
                if (account->password_hash == entered_hash) {
                    /* Level 4: Privilege level authorization */
                    if (account->security_level >= required_level) {
                        return AUTH_SUCCESS;
                    } else {
                        return AUTH_ERR_INSUFFICIENT_PERMISSIONS;
                    }
                } else {
                    return AUTH_ERR_INVALID_PASSWORD;
                }
            } else {
                return AUTH_ERR_ACCOUNT_LOCKED;
            }
        } else {
            return AUTH_ERR_USER_NOT_FOUND;
        }
    } else {
        return AUTH_ERR_USER_NOT_FOUND;
    }
}

/**
 * @brief Classifies loan eligibility based on income, credit score, and collateral.
 * @param annual_income In USD.
 * @param credit_score Standard FICO score (300-850).
 * @param has_collateral Boolean flag indicating pledged asset.
 * @return Descriptive string of the loan determination.
 */
const char* evaluate_loan_risk_tier(double annual_income, int credit_score, bool has_collateral) {
    if (annual_income >= 50000.0) {
        if (credit_score >= 720) {
            if (has_collateral) {
                return "TIER_1_PRIME: Lowest Interest Rate (3.2%)";
            } else {
                return "TIER_2_STANDARD: Good Interest Rate (4.5%)";
            }
        } else {
            if (has_collateral) {
                return "TIER_3_SECURED: Moderate Interest Rate (6.8%)";
            } else {
                return "TIER_4_SUBPRIME: High Risk Interest Rate (11.5%)";
            }
        }
    } else {
        if (credit_score >= 750 && has_collateral) {
            return "TIER_3_MICRO_SECURED: Approved with strict cap";
        } else {
            return "REJECTED: Income threshold unmet without exceptional collateral";
        }
    }
}

int main(void) {
    printf("===================================================================\n");
    printf("        DAY 11: NESTED IF-ELSE DECISION SYSTEMS BENCHMARK          \n");
    printf("===================================================================\n\n");

    /* Mock Database of Accounts */
    UserAccount users[] = {
        {.user_id = 101, .is_active = true,  .is_locked = false, .security_level = 3, .password_hash = 0x5F1A},
        {.user_id = 102, .is_active = true,  .is_locked = true,  .security_level = 2, .password_hash = 0x9B2C},
        {.user_id = 103, .is_active = false, .is_locked = false, .security_level = 1, .password_hash = 0x1111},
        {.user_id = 104, .is_active = true,  .is_locked = false, .security_level = 1, .password_hash = 0x3333}
    };
    size_t num_users = sizeof(users) / sizeof(users[0]);

    printf("[TEST SUITE 1: Multi-Tier Authentication Matrix]\n");
    for (size_t i = 0; i < num_users; ++i) {
        int test_hash = (i == 0) ? 0x5F1A : (i == 1 ? 0x9B2C : 0x0000);
        int req_level = 2;

        AuthResult res = evaluate_user_access_nested(&users[i], test_hash, req_level);
        printf("User ID %d -> Result: ", users[i].user_id);

        if (res == AUTH_SUCCESS) {
            printf("[ACCESS GRANTED] User verified for Level %d access.\n", req_level);
        } else {
            printf("[ACCESS DENIED] Reason Code: ");
            if (res == AUTH_ERR_USER_NOT_FOUND) printf("User Inactive or Not Found\n");
            else if (res == AUTH_ERR_ACCOUNT_LOCKED) printf("Account Locked\n");
            else if (res == AUTH_ERR_INVALID_PASSWORD) printf("Invalid Password\n");
            else if (res == AUTH_ERR_INSUFFICIENT_PERMISSIONS) printf("Insufficient Privilege Level\n");
        }
    }

    printf("\n[TEST SUITE 2: Multi-Factor Loan Risk Assessment]\n");
    struct {
        double income;
        int score;
        bool collateral;
    } loan_applicants[] = {
        {85000.0, 760, true},
        {60000.0, 730, false},
        {55000.0, 640, true},
        {35000.0, 780, true},
        {25000.0, 580, false}
    };
    size_t num_applicants = sizeof(loan_applicants) / sizeof(loan_applicants[0]);

    for (size_t j = 0; j < num_applicants; ++j) {
        const char *verdict = evaluate_loan_risk_tier(
            loan_applicants[j].income,
            loan_applicants[j].score,
            loan_applicants[j].collateral
        );
        printf("Applicant #%zu: Income=$%.2f, Score=%d, Collateral=%s\n  -> %s\n",
               j + 1,
               loan_applicants[j].income,
               loan_applicants[j].score,
               loan_applicants[j].collateral ? "YES" : "NO",
               verdict);
    }

    printf("\n===================================================================\n");
    printf("Execution completed successfully with zero memory leaks.\n");
    return 0;
}
