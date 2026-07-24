/**
 * ============================================================================
 * Day 48: Binary File I/O: fread(), fwrite(), fseek(), and ftell()
 * File: 48_Binary_Files_Fread_Fwrite_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 48_Binary_Files_Fread_Fwrite_code.c -o 48_binary_io
 *
 * Concepts Demonstrated:
 *   1. Writing and reading raw binary records with fwrite() and fread().
 *   2. Determining exact file size using fseek() and ftell().
 *   3. Instant random access to structured records at arbitrary index offsets.
 *   4. Detecting stream end and diagnosing read/write errors.
 *
 * Complexity:
 *   - Sequential Binary Write/Read: O(N * sizeof(Record)) time.
 *   - Random Access Record Lookup: O(1) time via fseek.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define BIN_FILENAME "day48_accounts.bin"

/* Fixed-size packed record structure suitable for binary serialization */
typedef struct {
    uint32_t account_number;
    char owner_name[32];
    double balance;
    uint32_t pin_hash;
} BankAccount;

/**
 * write_accounts_binary - Writes an array of BankAccount records to a binary file.
 * @param filename: Output binary file path.
 * @param accounts: Array of structures.
 * @param count: Number of elements.
 * @return: true on success, false on error.
 */
bool write_accounts_binary(const char *filename, const BankAccount *accounts, size_t count) {
    if (filename == NULL || accounts == NULL || count == 0) {
        return false;
    }

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("Error opening binary file for write");
        return false;
    }

    /* Write entire block of structures in a single high-speed system call */
    size_t written = fwrite(accounts, sizeof(BankAccount), count, fp);
    if (written != count) {
        perror("Error during binary fwrite");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/**
 * get_file_size_bytes - Computes file size using fseek and ftell.
 * @param filename: Target file path.
 * @return: Size in bytes, or -1 on error.
 */
long get_file_size_bytes(const char *filename) {
    if (filename == NULL) return -1;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return -1;

    /* Move to end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    long size = ftell(fp);
    fclose(fp);
    return size;
}

/**
 * read_account_at_index - Random-access lookup of a record at specific index.
 * @param filename: Binary database file.
 * @param index: Zero-based record index.
 * @param out_account: Destination struct pointer.
 * @return: true if record read successfully, false otherwise.
 */
bool read_account_at_index(const char *filename, size_t index, BankAccount *out_account) {
    if (filename == NULL || out_account == NULL) return false;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening binary file for random read");
        return false;
    }

    /* Compute offset: index * sizeof(Record) */
    long offset = (long)(index * sizeof(BankAccount));
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }

    size_t read_count = fread(out_account, sizeof(BankAccount), 1, fp);
    fclose(fp);

    return (read_count == 1);
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 48: BINARY FILE I/O (FREAD, FWRITE, FSEEK)   \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Creating and writing binary records
     * ------------------------------------------------------------------------ */
    BankAccount database[] = {
        { 10001, "Alice Smith",    5420.50, 0xA1B2C3D4 },
        { 10002, "Bob Johnson",   12800.75, 0x11223344 },
        { 10003, "Charlie Davis",   950.00, 0x55667788 },
        { 10004, "Diana Prince",  45000.25, 0xDEADBEEF }
    };
    const size_t total_records = sizeof(database) / sizeof(database[0]);

    printf("[1] Writing %zu binary records to '%s'...\n", total_records, BIN_FILENAME);
    if (write_accounts_binary(BIN_FILENAME, database, total_records)) {
        printf("    Binary database successfully written.\n");
    }

    /* ------------------------------------------------------------------------
     * Section 2: Calculating binary file size
     * ------------------------------------------------------------------------ */
    long file_size = get_file_size_bytes(BIN_FILENAME);
    printf("\n[2] Binary File Size Metric:\n");
    printf("    Total Size on Disk: %ld bytes\n", file_size);
    printf("    Expected Size: %zu bytes (%zu records * %zu bytes/record)\n",
           total_records * sizeof(BankAccount), total_records, sizeof(BankAccount));

    /* ------------------------------------------------------------------------
     * Section 3: Random Access Lookup via fseek()
     * ------------------------------------------------------------------------ */
    printf("\n[3] Random Access Record Lookup (Direct Seeking):\n");
    size_t target_indices[] = { 2, 0, 3 };

    for (size_t i = 0; i < sizeof(target_indices) / sizeof(target_indices[0]); ++i) {
        size_t idx = target_indices[i];
        BankAccount target;

        if (read_account_at_index(BIN_FILENAME, idx, &target)) {
            printf("    Index [%zu] -> Acc#: %u | Owner: %-14s | Balance: $%.2f | Hash: 0x%08X\n",
                   idx, target.account_number, target.owner_name, target.balance, target.pin_hash);
        } else {
            fprintf(stderr, "    Failed to seek to index %zu\n", idx);
        }
    }

    /* ------------------------------------------------------------------------
     * Section 4: Cleanup Temporary Binary File
     * ------------------------------------------------------------------------ */
    printf("\n[4] Deleting temporary binary file:\n");
    if (remove(BIN_FILENAME) == 0) {
        printf("    '%s' removed cleanly.\n", BIN_FILENAME);
    } else {
        perror("    Failed to remove binary test file");
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}
