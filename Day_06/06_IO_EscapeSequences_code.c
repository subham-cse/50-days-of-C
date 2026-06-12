/**
 * ============================================================================
 * DAY 06: Standard I/O, Escape Sequences, and Safe Formatted Streams
 * ============================================================================
 * Description:
 *   Demonstrates formatted standard I/O (printf, fprintf, sscanf, snprintf),
 *   stream buffering behaviors, field width/precision alignments, escape
 *   sequences, and format string safety patterns.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 06_IO_EscapeSequences_code.c -o day06_io
 *
 * Execution:
 *   ./day06_io
 *
 * Complexity Metrics:
 *   - Time Complexity: O(N) where N is number of formatted table rows
 *   - Space Complexity: O(M) bounded buffer size for string formatting
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * 1. STRUCTURES & CONSTANTS
 * ============================================================================
 */

typedef struct SensorTelemetry {
    uint32_t sensor_id;
    char     sensor_name[32];
    double   temperature_celsius;
    double   pressure_kpa;
    bool     is_active;
} SensorTelemetry_t;

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * render_telemetry_table:
 *   Demonstrates advanced printf formatting flags: explicit field widths,
 *   left alignment (-), zero padding (0), precision specifiers, and escape tabs.
 *
 * Parameters:
 *   readings - Array of telemetry records.
 *   count    - Number of elements in array.
 *
 * Returns:
 *   void
 */
void render_telemetry_table(const SensorTelemetry_t *readings, size_t count)
{
    printf("\n==================== TELEMETRY REPORT TABLE ====================\n");
    printf("| %-8s | %-16s | %-10s | %-10s | %-6s |\n",
           "ID", "SENSOR NAME", "TEMP (C)", "PRESS(kPa)", "STATUS");
    printf("+----------+------------------+------------+------------+--------+\n");

    for (size_t i = 0; i < count; ++i) {
        printf("| %08u | %-16.16s | %+10.2f | %10.3f | %-6s |\n",
               readings[i].sensor_id,
               readings[i].sensor_name,
               readings[i].temperature_celsius,
               readings[i].pressure_kpa,
               readings[i].is_active ? "ACTIVE" : "OFF");
    }
    printf("================================================================\n\n");
}

/**
 * parse_telemetry_record_safe:
 *   Demonstrates bounded safe formatted string extraction using sscanf
 *   with strict width guards and return-value verification.
 *
 * Parameters:
 *   input_str - Raw serialized text record (e.g. "101,CoreTemp,42.50,101.325,1").
 *   out_rec   - Pointer to destination telemetry struct.
 *
 * Returns:
 *   bool - True if all fields parsed with 100% integrity, False on failure.
 */
bool parse_telemetry_record_safe(const char *input_str, SensorTelemetry_t *out_rec)
{
    if (input_str == NULL || out_rec == NULL) {
        return false;
    }

    int active_flag = 0;
    /* Strict format string with maximum 31 characters for string buffer to prevent overflow */
    int matched = sscanf(input_str, "%u,%31[^,],%lf,%lf,%d",
                         &out_rec->sensor_id,
                         out_rec->sensor_name,
                         &out_rec->temperature_celsius,
                         &out_rec->pressure_kpa,
                         &active_flag);

    if (matched != 5) {
        fprintf(stderr, "[ERROR] sscanf failed! Expected 5 fields, matched %d in: \"%s\"\n",
                matched, input_str);
        return false;
    }

    out_rec->is_active = (active_flag != 0);
    return true;
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 06: Standard I/O, Escape Sequences, and Format Flags\n");
    printf("==============================================================\n");

    /* 1. Escape Sequence Demonstrations */
    printf("--- 1. Escape Sequence Diagnostics ---\n");
    printf("  [\\n] Line Feed -> Newline rendered.\n");
    printf("  [\\t] Tabular Alignment: [Col1]\t[Col2]\t[Col3]\n");
    printf("  [\\\\] Literal Backslash: C:\\Program Files\\LLVM\\bin\n");
    printf("  [\\\"] Escaped Quotes: \"Strict ANSI C Standard Conformity\"\n");
    printf("  [\\x1B] ANSI Color Control: \x1B[32m[OK - Green Text]\x1B[0m\n\n");

    /* 2. Safe Parsing Test */
    printf("--- 2. Safe String Stream Deserialization ---\n");
    const char *raw_data[] = {
        "1001,PrimaryCore,68.450,101.325,1",
        "1002,SecondaryCore,-12.300,98.210,1",
        "1003,AuxiliaryHeater,125.800,104.550,0"
    };
    const size_t num_records = sizeof(raw_data) / sizeof(raw_data[0]);

    SensorTelemetry_t records[3];
    for (size_t i = 0; i < num_records; ++i) {
        if (parse_telemetry_record_safe(raw_data[i], &records[i])) {
            printf("  Successfully parsed record: %s\n", records[i].sensor_name);
        }
    }

    /* 3. Render Formatted Output Table */
    render_telemetry_table(records, num_records);

    /* 4. Stream Redirection and Flushing */
    fprintf(stdout, "[STDOUT] Standard output stream (Line buffered)\n");
    fprintf(stderr, "[STDERR] Standard error stream (Unbuffered immediate flush)\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
