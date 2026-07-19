/**
 * ============================================================================
 * Day 43: Dynamic Memory Allocation with malloc() and free()
 * File: 43_Malloc_Free_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 43_Malloc_Free_code.c -o 43_malloc_free
 *
 * Concepts Demonstrated:
 *   1. Heap allocation of primitive arrays and custom structures.
 *   2. Strict error handling (OOM checks) and graceful recovery.
 *   3. Complete resource cleanup without memory leaks.
 *   4. Safe dynamic string duplication.
 *
 * Complexity:
 *   - Allocation/Free: O(1) amortized time.
 *   - Array/String Initialization: O(N) time.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Structure representing an employee record in dynamic storage */
typedef struct {
    int id;
    char *name;
    double salary;
} Employee;

/**
 * create_employee - Allocates and initializes an Employee structure on the heap.
 * @param id: Employee ID.
 * @param name: Employee full name (will be duplicated onto heap).
 * @param salary: Employee salary.
 * @return: Pointer to allocated Employee, or NULL on allocation failure.
 */
Employee *create_employee(int id, const char *name, double salary) {
    if (name == NULL) {
        return NULL;
    }

    /* Allocate structure */
    Employee *emp = malloc(sizeof(Employee));
    if (emp == NULL) {
        return NULL; /* Out of memory */
    }

    /* Allocate string buffer for name (+1 for null terminator) */
    size_t name_len = strlen(name);
    emp->name = malloc(name_len + 1);
    if (emp->name == NULL) {
        free(emp); /* Clean up previously allocated struct to prevent leak */
        return NULL;
    }

    memcpy(emp->name, name, name_len + 1);
    emp->id = id;
    emp->salary = salary;

    return emp;
}

/**
 * destroy_employee - Recursively frees heap allocations of an Employee.
 * @param emp: Pointer to employee structure.
 */
void destroy_employee(Employee *emp) {
    if (emp == NULL) {
        return;
    }
    if (emp->name != NULL) {
        free(emp->name);
        emp->name = NULL;
    }
    free(emp);
}

/**
 * create_int_array - Allocates a contiguous dynamic integer array.
 * @param count: Number of elements.
 * @return: Pointer to allocated memory, or NULL on failure.
 */
int *create_int_array(size_t count) {
    if (count == 0 || count > (size_t)-1 / sizeof(int)) {
        return NULL; /* Guard against zero length and integer overflow */
    }

    int *arr = malloc(count * sizeof(int));
    if (arr == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < count; ++i) {
        arr[i] = (int)((i + 1) * 7);
    }

    return arr;
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 43: DYNAMIC MEMORY ALLOCATION (MALLOC/FREE)  \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Dynamic Integer Array Allocation
     * ------------------------------------------------------------------------ */
    printf("[1] Dynamic Array Allocation via malloc():\n");
    const size_t array_size = 6;
    int *numbers = create_int_array(array_size);

    if (numbers != NULL) {
        printf("    Allocated %zu integers at address: %p\n", array_size, (void *)numbers);
        printf("    Values: ");
        for (size_t i = 0; i < array_size; ++i) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        /* Deallocate array */
        free(numbers);
        numbers = NULL;
        printf("    Buffer deallocated successfully.\n\n");
    } else {
        fprintf(stderr, "    Failed to allocate integer array.\n");
    }

    /* ------------------------------------------------------------------------
     * Section 2: Deep Heap Structure Allocation & Teardown
     * ------------------------------------------------------------------------ */
    printf("[2] Deep Heap Structure Allocation (Employee Records):\n");
    Employee *dev1 = create_employee(101, "Ada Lovelace", 125000.50);
    Employee *dev2 = create_employee(102, "Alan Turing", 140000.00);

    if (dev1 != NULL && dev2 != NULL) {
        printf("    Record 1: ID=%d, Name=\"%s\", Salary=$%.2f (Address=%p)\n",
               dev1->id, dev1->name, dev1->salary, (void *)dev1);
        printf("    Record 2: ID=%d, Name=\"%s\", Salary=$%.2f (Address=%p)\n",
               dev2->id, dev2->name, dev2->salary, (void *)dev2);

        /* Clean up individual structures */
        destroy_employee(dev1);
        dev1 = NULL;
        destroy_employee(dev2);
        dev2 = NULL;
        printf("    All employee heap records successfully destroyed.\n\n");
    } else {
        fprintf(stderr, "    Failed to allocate employee records.\n");
        destroy_employee(dev1);
        destroy_employee(dev2);
    }

    /* ------------------------------------------------------------------------
     * Section 3: Safe free(NULL) Verification
     * ------------------------------------------------------------------------ */
    printf("[3] Verifying free(NULL) Safety:\n");
    void *null_ptr = NULL;
    free(null_ptr); /* Guaranteed safe by ANSI C */
    printf("    free(NULL) executed safely with zero side-effects.\n");

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}
