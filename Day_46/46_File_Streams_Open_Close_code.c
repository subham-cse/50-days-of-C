/**
 * ============================================================================
 * Day 46: File Streams, File Descriptors, fopen(), and fclose()
 * File: 46_File_Streams_Open_Close_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 46_File_Streams_Open_Close_code.c -o 46_file_streams
 *
 * Concepts Demonstrated:
 *   1. Robust file creation, writing, appending, and reading with fopen()/fclose().
 *   2. Strict error diagnosis with perror() and errno.
 *   3. Demonstrating user-space buffer flushing with fflush().
 *   4. Clean temporary file cleanup to prevent resource leaks.
 *
 * Complexity:
 *   - File Open/Close: O(1) OS operations.
 *   - File Stream Processing: O(N) where N is number of bytes processed.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define TEMP_LOG_FILE "day46_stream_test.txt"

/**
 * write_initial_log - Creates or truncates a file and writes initial records.
 * @param filename: Target file path.
 * @return: true on success, false on error.
 */
bool write_initial_log(const char *filename) {
    if (filename == NULL) return false;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file for write");
        return false;
    }

    /* Write structured log entries */
    fputs("[SYSTEM] Kernel initialization complete.\n", fp);
    fputs("[INFO] Service worker started on port 8080.\n", fp);

    /* Explicit flush to ensure user-space buffer is flushed */
    if (fflush(fp) != 0) {
        perror("Error flushing stream buffer");
        fclose(fp);
        return false;
    }

    if (fclose(fp) != 0) {
        perror("Error closing file after write");
        return false;
    }

    return true;
}

/**
 * append_log_entry - Appends a message to the file without truncating existing data.
 * @param filename: Target file path.
 * @param entry: Message to append.
 * @return: true on success, false on error.
 */
bool append_log_entry(const char *filename, const char *entry) {
    if (filename == NULL || entry == NULL) return false;

    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Error opening file for append");
        return false;
    }

    fputs(entry, fp);
    fputc('\n', fp);

    if (fclose(fp) != 0) {
        perror("Error closing file after append");
        return false;
    }

    return true;
}

/**
 * read_and_display_file - Reads and prints entire file contents character by character.
 * @param filename: Target file path.
 * @return: Number of bytes read, or -1 on error.
 */
long read_and_display_file(const char *filename) {
    if (filename == NULL) return -1;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file for read");
        return -1;
    }

    printf("    --- Begin File Content (%s) ---\n", filename);
    int ch;
    long byte_count = 0;

    /* Read character by character until EOF */
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
        byte_count++;
    }

    /* Check whether loop terminated due to EOF or stream read error */
    if (ferror(fp)) {
        fprintf(stderr, "Stream error encountered while reading %s\n", filename);
        fclose(fp);
        return -1;
    }

    printf("    --- End File Content (%ld bytes read) ---\n", byte_count);

    if (fclose(fp) != 0) {
        perror("Error closing file after read");
        return -1;
    }

    return byte_count;
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 46: FILE STREAMS, FOPEN & FCLOSE             \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Attempting to open a non-existent file in "r" mode
     * ------------------------------------------------------------------------ */
    printf("[1] Error Handling Test (Non-existent file):\n");
    FILE *missing_fp = fopen("non_existent_file_99999.xyz", "r");
    if (missing_fp == NULL) {
        printf("    Safely caught expected error: errno = %d (%s)\n\n",
               errno, strerror(errno));
    } else {
        fclose(missing_fp);
    }

    /* ------------------------------------------------------------------------
     * Section 2: Create & Write Log File in "w" Mode
     * ------------------------------------------------------------------------ */
    printf("[2] Writing initial log entries using \"w\" mode:\n");
    if (write_initial_log(TEMP_LOG_FILE)) {
        printf("    File '%s' successfully created and written.\n\n", TEMP_LOG_FILE);
    }

    /* ------------------------------------------------------------------------
     * Section 3: Appending Data in "a" Mode
     * ------------------------------------------------------------------------ */
    printf("[3] Appending new log entries using \"a\" mode:\n");
    append_log_entry(TEMP_LOG_FILE, "[WARNING] High memory pressure detected: 82% threshold.");
    append_log_entry(TEMP_LOG_FILE, "[SUCCESS] Auto-scale worker spawned.");
    printf("    Appended 2 log records successfully.\n\n");

    /* ------------------------------------------------------------------------
     * Section 4: Reading and Displaying Full Stream
     * ------------------------------------------------------------------------ */
    printf("[4] Reading stream using \"r\" mode:\n");
    read_and_display_file(TEMP_LOG_FILE);

    /* ------------------------------------------------------------------------
     * Section 5: Cleanup Temporary Disk Artifact
     * ------------------------------------------------------------------------ */
    printf("\n[5] Cleaning up temporary test file:\n");
    if (remove(TEMP_LOG_FILE) == 0) {
        printf("    Temporary file '%s' successfully deleted from disk.\n", TEMP_LOG_FILE);
    } else {
        perror("    Warning: Failed to delete temporary file");
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}
