/**
 * ============================================================================
 * Day 47: Formatted File I/O: fprintf(), fscanf(), fgets(), and sscanf()
 * File: 47_Formatted_IO_Fprintf_Fscanf_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 47_Formatted_IO_Fprintf_Fscanf_code.c -o 47_formatted_io
 *
 * Concepts Demonstrated:
 *   1. Writing structured CSV records with fprintf().
 *   2. The robust production pattern: fgets() line reading + sscanf() parsing.
 *   3. Scanset specifiers `%[^,]` for parsing strings with spaces and commas.
 *   4. Handling malformed input rows and file error conditions.
 *
 * Complexity:
 *   - Writing: O(N) where N is number of records.
 *   - Parsing: O(M) where M is total line characters.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CSV_FILENAME "day47_students.csv"
#define MAX_LINE_LEN 256
#define MAX_NAME_LEN 64

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    double gpa;
    int graduation_year;
} StudentRecord;

/**
 * write_student_csv - Exports an array of student records to a CSV file.
 * @param filename: Destination CSV path.
 * @param records: Array of StudentRecord structures.
 * @param count: Number of records.
 * @return: true on success, false on failure.
 */
bool write_student_csv(const char *filename, const StudentRecord *records, size_t count) {
    if (filename == NULL || records == NULL || count == 0) {
        return false;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening CSV for writing");
        return false;
    }

    /* Write CSV header */
    fprintf(fp, "ID,Name,GPA,GraduationYear\n");

    /* Write formatted data rows */
    for (size_t i = 0; i < count; ++i) {
        fprintf(fp, "%d,%s,%.2f,%d\n",
                records[i].id,
                records[i].name,
                records[i].gpa,
                records[i].graduation_year);
    }

    if (fclose(fp) != 0) {
        perror("Error closing CSV after write");
        return false;
    }

    return true;
}

/**
 * read_student_csv - Reads and parses student records using fgets() + sscanf().
 * @param filename: Source CSV path.
 * @param out_records: Destination array for parsed records.
 * @param max_records: Maximum capacity of destination array.
 * @return: Number of successfully parsed records.
 */
size_t read_student_csv(const char *filename, StudentRecord *out_records, size_t max_records) {
    if (filename == NULL || out_records == NULL || max_records == 0) {
        return 0;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening CSV for reading");
        return 0;
    }

    char line_buffer[MAX_LINE_LEN];
    size_t record_count = 0;
    bool is_header = true;

    /* Read line-by-line safely */
    while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        /* Skip header line */
        if (is_header) {
            is_header = false;
            continue;
        }

        if (record_count >= max_records) {
            printf("    [Warning] Maximum buffer capacity reached (%zu records).\n", max_records);
            break;
        }

        StudentRecord temp;
        /* Scanset %63[^,] parses everything up to the next comma safely */
        int matched = sscanf(line_buffer, "%d,%63[^,],%lf,%d",
                             &temp.id,
                             temp.name,
                             &temp.gpa,
                             &temp.graduation_year);

        if (matched == 4) {
            out_records[record_count++] = temp;
        } else {
            fprintf(stderr, "    [Skipping Malformed Line]: %s", line_buffer);
        }
    }

    fclose(fp);
    return record_count;
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 47: FORMATTED FILE I/O (FPRINTF & SSCANF)    \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Creating Sample Data & Exporting to CSV
     * ------------------------------------------------------------------------ */
    StudentRecord dataset[] = {
        { 1001, "Grace Hopper",    3.95, 2024 },
        { 1002, "Claude Shannon",   4.00, 2025 },
        { 1003, "Dennis Ritchie",  3.88, 2024 },
        { 1004, "Ken Thompson",    3.91, 2026 }
    };
    const size_t dataset_size = sizeof(dataset) / sizeof(dataset[0]);

    printf("[1] Writing %zu structured records to '%s':\n", dataset_size, CSV_FILENAME);
    if (write_student_csv(CSV_FILENAME, dataset, dataset_size)) {
        printf("    CSV database exported successfully.\n\n");
    }

    /* ------------------------------------------------------------------------
     * Section 2: Reading & Parsing CSV using fgets() + sscanf()
     * ------------------------------------------------------------------------ */
    printf("[2] Parsing CSV database back into memory:\n");
    StudentRecord imported_data[10];
    size_t imported_count = read_student_csv(CSV_FILENAME, imported_data, 10);

    printf("    Successfully parsed %zu records:\n", imported_count);
    printf("    +------+----------------------+------+------+\n");
    printf("    |  ID  | Name                 | GPA  | Year |\n");
    printf("    +------+----------------------+------+------+\n");
    for (size_t i = 0; i < imported_count; ++i) {
        printf("    | %4d | %-20s | %4.2f | %4d |\n",
               imported_data[i].id,
               imported_data[i].name,
               imported_data[i].gpa,
               imported_data[i].graduation_year);
    }
    printf("    +------+----------------------+------+------+\n\n");

    /* ------------------------------------------------------------------------
     * Section 3: Clean up temporary test file
     * ------------------------------------------------------------------------ */
    printf("[3] Deleting temporary CSV file:\n");
    if (remove(CSV_FILENAME) == 0) {
        printf("    '%s' removed cleanly.\n", CSV_FILENAME);
    } else {
        perror("    Failed to remove test file");
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}
