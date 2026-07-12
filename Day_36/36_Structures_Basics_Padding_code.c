/**
 * ============================================================================
 * Day 36: Structure Basics, Memory Alignment, Padding & offsetof
 * File: 36_Structures_Basics_Padding_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 36_Structures_Basics_Padding_code.c -o 36_Structures_Basics_Padding_code
 * 
 * Description:
 *   Examines member offsets using offsetof(), demonstrates compiler padding
 *   effects on struct size, compares unoptimized vs reordered vs packed structs,
 *   and prints raw memory byte layouts.
 * 
 * Complexity:
 *   Time Complexity:  O(1) struct access; O(N) byte inspection
 *   Space Complexity: Contiguous memory layouts
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* 1. Unoptimized struct with suboptimal member ordering */
struct BadAlignment {
    char   flag1;  /* 1 byte  - offset 0 */
                   /* 3 padding bytes */
    int    id;     /* 4 bytes - offset 4 */
    char   flag2;  /* 1 byte  - offset 8 */
                   /* 7 padding bytes */
    double salary; /* 8 bytes - offset 16 */
};

/* 2. Reordered optimized struct (sorted in descending order of size) */
struct GoodAlignment {
    double salary; /* 8 bytes - offset 0 */
    int    id;     /* 4 bytes - offset 8 */
    char   flag1;  /* 1 byte  - offset 12 */
    char   flag2;  /* 1 byte  - offset 13 */
                   /* 2 tail padding bytes to reach multiple of 8 */
};

/* 3. Packed struct (forcing 1-byte alignment, no padding) */
#pragma pack(push, 1)
struct PackedStruct {
    char   flag1;  /* 1 byte  - offset 0 */
    int    id;     /* 4 bytes - offset 1 */
    char   flag2;  /* 1 byte  - offset 5 */
    double salary; /* 8 bytes - offset 6 */
};
#pragma pack(pop)

/**
 * @brief Prints hexadecimal byte dump of any memory structure.
 * 
 * @param ptr Pointer to data block.
 * @param size Number of bytes to print.
 * @param label Description label.
 */
void dump_memory_bytes(const void *ptr, size_t size, const char *label) {
    const unsigned char *byte_ptr = (const unsigned char *)ptr;
    printf("Memory Byte Dump for '%s' (%zu bytes total):\n  Address: %p\n  Bytes: [ ", label, size, ptr);
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", byte_ptr[i]);
    }
    printf("]\n");
}

/**
 * @brief Analyzes and prints member offsets and total size of BadAlignment struct.
 */
void inspect_bad_alignment(void) {
    printf("--- [Analysis of Unoptimized Struct] ---\n");
    printf("sizeof(struct BadAlignment) = %zu bytes\n", sizeof(struct BadAlignment));
    printf("  offsetof(flag1)  = %zu\n", offsetof(struct BadAlignment, flag1));
    printf("  offsetof(id)     = %zu (Padding before id: %zu bytes)\n", 
           offsetof(struct BadAlignment, id), offsetof(struct BadAlignment, id) - sizeof(char));
    printf("  offsetof(flag2)  = %zu\n", offsetof(struct BadAlignment, flag2));
    printf("  offsetof(salary) = %zu (Padding before salary: %zu bytes)\n\n", 
           offsetof(struct BadAlignment, salary), 
           offsetof(struct BadAlignment, salary) - (offsetof(struct BadAlignment, flag2) + sizeof(char)));
}

/**
 * @brief Analyzes and prints member offsets and total size of GoodAlignment struct.
 */
void inspect_good_alignment(void) {
    printf("--- [Analysis of Optimized Reordered Struct] ---\n");
    printf("sizeof(struct GoodAlignment) = %zu bytes\n", sizeof(struct GoodAlignment));
    printf("  offsetof(salary) = %zu\n", offsetof(struct GoodAlignment, salary));
    printf("  offsetof(id)     = %zu\n", offsetof(struct GoodAlignment, id));
    printf("  offsetof(flag1)  = %zu\n", offsetof(struct GoodAlignment, flag1));
    printf("  offsetof(flag2)  = %zu\n", offsetof(struct GoodAlignment, flag2));
    printf("  Memory saved compared to unoptimized: %zu bytes (%.1f%% reduction)\n\n",
           sizeof(struct BadAlignment) - sizeof(struct GoodAlignment),
           ((double)(sizeof(struct BadAlignment) - sizeof(struct GoodAlignment)) / sizeof(struct BadAlignment)) * 100.0);
}

/**
 * @brief Compares struct instances and highlights padding garbage dangers.
 */
void demonstrate_padding_garbage_peril(void) {
    struct BadAlignment s1, s2;
    
    /* Zero out s1 entirely */
    memset(&s1, 0, sizeof(struct BadAlignment));
    s1.flag1 = 'A';
    s1.id = 101;
    s1.flag2 = 'B';
    s1.salary = 75000.50;

    /* Fill s2 with 0xFF junk bits first, then assign identical member values */
    memset(&s2, 0xFF, sizeof(struct BadAlignment));
    s2.flag1 = 'A';
    s2.id = 101;
    s2.flag2 = 'B';
    s2.salary = 75000.50;

    printf("--- [Demonstrating Perils of memcmp() on Structs with Padding] ---\n");
    printf("Comparing s1 and s2 fields:\n");
    printf("  s1: flag1='%c', id=%d, flag2='%c', salary=%.2f\n", s1.flag1, s1.id, s1.flag2, s1.salary);
    printf("  s2: flag1='%c', id=%d, flag2='%c', salary=%.2f\n", s2.flag1, s2.id, s2.flag2, s2.salary);

    int cmp_result = memcmp(&s1, &s2, sizeof(struct BadAlignment));
    printf("memcmp(&s1, &s2, sizeof(struct BadAlignment)) returned: %d\n", cmp_result);
    if (cmp_result != 0) {
        printf("CRITICAL WARNING: memcmp failed even though all member fields are identical!\n");
        printf("Reason: Uninitialized padding bytes contain divergent bits.\n\n");
    }
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 36: STRUCTURES — MEMORY ALIGNMENT & PADDING\n");
    printf("===============================================================\n\n");

    /* 1. Unoptimized vs Optimized inspection */
    inspect_bad_alignment();
    inspect_good_alignment();

    /* 2. Packed Struct inspection */
    printf("--- [Analysis of Packed Struct (#pragma pack(1))] ---\n");
    printf("sizeof(struct PackedStruct) = %zu bytes (Zero wasted padding)\n", sizeof(struct PackedStruct));
    printf("  offsetof(flag1)  = %zu\n", offsetof(struct PackedStruct, flag1));
    printf("  offsetof(id)     = %zu\n", offsetof(struct PackedStruct, id));
    printf("  offsetof(flag2)  = %zu\n", offsetof(struct PackedStruct, flag2));
    printf("  offsetof(salary) = %zu\n\n", offsetof(struct PackedStruct, salary));

    /* 3. Memory byte dump */
    struct GoodAlignment good_example = {
        .salary = 12345.67,
        .id = 0x12345678,
        .flag1 = 0xAA,
        .flag2 = 0xBB
    };
    dump_memory_bytes(&good_example, sizeof(good_example), "GoodAlignment Instance");
    printf("\n");

    /* 4. Padding garbage pitfall */
    demonstrate_padding_garbage_peril();

    return 0;
}
