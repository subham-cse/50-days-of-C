/**
 * ============================================================================
 * Day 37: Unions vs. Structures — Shared Memory & Tagged Unions
 * File: 37_Unions_vs_Structures_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 37_Unions_vs_Structures_code.c -o 37_Unions_vs_Structures_code
 * 
 * Description:
 *   Demonstrates memory address sharing in unions, size differences between
 *   structs and unions, Tagged (Discriminated) Union variant pattern,
 *   CPU Endianness detection, and IEEE-754 single-precision float bit inspection.
 * 
 * Complexity:
 *   Time Complexity:  O(1) field accesses
 *   Space Complexity: O(1) shared memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Type tag enumeration for Tagged Union */
typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

/* Tagged / Discriminated Union pattern */
typedef struct {
    DataType tag;
    union {
        int64_t i_val;
        double  f_val;
        char    str_val[32];
    } payload;
} DynamicVariant;

/* Union for IEEE-754 Single-Precision Float Bit Inspection (Type Punning) */
typedef union {
    float    f;
    uint32_t u;
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign     : 1;
    } parts;
} FloatInspector;

/* Union for Hardware Endianness Detection */
typedef union {
    uint32_t integer;
    uint8_t  bytes[4];
} EndianChecker;

/**
 * @brief Prints a Tagged Variant safely by checking its active tag.
 * 
 * @param v Pointer to variant.
 */
void print_variant(const DynamicVariant *v) {
    if (!v) return;

    printf("Variant at %p (tag: %d) -> Value: ", (const void*)v, v->tag);
    switch (v->tag) {
        case TYPE_INTEGER:
            printf("[Integer] %lld\n", (long long)v->payload.i_val);
            break;
        case TYPE_FLOAT:
            printf("[Float/Double] %.6f\n", v->payload.f_val);
            break;
        case TYPE_STRING:
            printf("[String] \"%s\"\n", v->payload.str_val);
            break;
        default:
            printf("[Unknown Type]\n");
            break;
    }
}

/**
 * @brief Demonstrates CPU Endianness detection via Union.
 */
void detect_system_endianness(void) {
    EndianChecker checker;
    checker.integer = 0x01020304;

    printf("--- [Hardware Endianness Detection] ---\n");
    printf("32-bit Integer Value: 0x%08X\n", checker.integer);
    printf("Raw Byte Sequence in Memory:\n");
    for (int i = 0; i < 4; ++i) {
        printf("  Byte [%d] at %p: 0x%02X\n", i, (void*)&checker.bytes[i], checker.bytes[i]);
    }

    if (checker.bytes[0] == 0x04) {
        printf("System Result: LITTLE-ENDIAN (Least Significant Byte stored first)\n\n");
    } else if (checker.bytes[0] == 0x01) {
        printf("System Result: BIG-ENDIAN (Most Significant Byte stored first)\n\n");
    } else {
        printf("System Result: UNUSUAL / MIXED ENDIAN\n\n");
    }
}

/**
 * @brief Demonstrates IEEE-754 float bit unpacking via union type punning.
 * 
 * @param val Floating point number to inspect.
 */
void inspect_float_bits(float val) {
    FloatInspector fi;
    fi.f = val;

    printf("--- [IEEE-754 Float Bit Inspection for %.3f] ---\n", val);
    printf("Float Value:     %.6f\n", fi.f);
    printf("Raw Hex (u32):   0x%08X\n", fi.u);
    printf("Sign Bit:        %u (%s)\n", fi.parts.sign, fi.parts.sign ? "Negative (-)" : "Positive (+)");
    printf("Exponent Bits:   0x%02X (Unbiased: %d)\n", fi.parts.exponent, (int)fi.parts.exponent - 127);
    printf("Mantissa Bits:   0x%06X\n\n", fi.parts.mantissa);
}

/**
 * @brief Demonstrates memory address sharing and member overwrite behavior.
 */
void demonstrate_union_overlap(void) {
    union ExampleUnion {
        int64_t  big_int;
        double   floating;
        char     text[8];
    } u;

    printf("--- [Union Shared Memory Address & Overwrite Demonstration] ---\n");
    printf("sizeof(union ExampleUnion) = %zu bytes\n", sizeof(u));
    printf("Address of u.big_int:  %p\n", (void*)&u.big_int);
    printf("Address of u.floating: %p\n", (void*)&u.floating);
    printf("Address of u.text:     %p\n", (void*)&u.text);
    printf("Notice: All members share the exact same starting byte address!\n\n");

    /* Step 1: Assign integer */
    u.big_int = 0x1122334455667788LL;
    printf("Assigned big_int = 0x%llX\n", (unsigned long long)u.big_int);

    /* Step 2: Assign double (overwrites big_int) */
    u.floating = 3.141592653589793;
    printf("Assigned floating = %.15f (Overwrites previous data)\n", u.floating);
    printf("Reading big_int now gives corrupted/punning value: 0x%llX\n\n", (unsigned long long)u.big_int);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 37: UNIONS VS. STRUCTURES & TAGGED VARIANTS\n");
    printf("===============================================================\n\n");

    /* 1. Shared address and overwriting */
    demonstrate_union_overlap();

    /* 2. Tagged Union Variant Pattern */
    printf("--- [Tagged (Discriminated) Union Variants] ---\n");
    DynamicVariant v1, v2, v3;

    v1.tag = TYPE_INTEGER;
    v1.payload.i_val = 9876543210LL;

    v2.tag = TYPE_FLOAT;
    v2.payload.f_val = 2.718281828459;

    v3.tag = TYPE_STRING;
    strncpy(v3.payload.str_val, "Antigravity C99 Core", sizeof(v3.payload.str_val) - 1);
    v3.payload.str_val[sizeof(v3.payload.str_val) - 1] = '\0';

    print_variant(&v1);
    print_variant(&v2);
    print_variant(&v3);
    printf("\n");

    /* 3. Hardware Endianness detection */
    detect_system_endianness();

    /* 4. Float bit inspection */
    inspect_float_bits(-12.375f);

    return 0;
}
