# Day 47: Formatted File I/O: `fprintf()`, `fscanf()`, `fgets()`, and `sscanf()`

## 1. Overview & Real-World Analogy
Formatted file I/O transforms human-readable text representations into in-memory binary data structures and vice-versa. While primitive I/O moves raw bytes, formatted I/O parses delimiters, radix strings, floating-point representations, and structured text protocols (such as CSV, TSV, and configuration files).

### Real-World Analogy
Imagine a customs declaration form:
- **`fprintf()`**: Fills out the form clearly with labels, integers, formatted decimals, and names so any inspector can read it.
- **`fscanf()`**: An automated scanner that extracts values into specific slots, but gets easily tripped up or confused if a traveler enters spaces where none were expected.
- **`fgets()` + `sscanf()`**: The robust two-stage verification approach: First, photocopy the entire page safely into a folder (`fgets`), then carefully inspect and extract fields from the photocopy without risking getting stuck on the original paper stream.

---

## 2. Deep Technical Dive & Format Specifier Grammar

### Signatures (`<stdio.h>`)
```c
int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
char *fgets(char *str, int n, FILE *stream);
int sscanf(const char *buffer, const char *format, ...);
```

### Format Specifier Architecture
`%[flags][width][.precision][length]specifier`

```
+-----------+-----------------------------------------------------------------+
| Component | Function / Examples                                             |
+-----------+-----------------------------------------------------------------+
| Flags     | '-' (left align), '+' (force sign), '0' (zero-pad)              |
| Width     | Minimum number of characters to output (e.g. %10d)              |
| Precision | Decimal places for floats (%.2f) or max string chars (%.5s)     |
| Length    | 'h' (short), 'l' (long), 'll' (long long), 'z' (size_t)         |
| Specifier | 'd'/'i' (int), 'u' (unsigned), 'f' (float), 's' (str), 'c' (chr)|
+-----------+-----------------------------------------------------------------+
```

---

## 3. Why Direct `fscanf()` on Streams is Dangerous

Direct use of `fscanf(fp, "%s %d", ...)` on arbitrary file input has severe structural pitfalls:
1. **Buffer Overflow Hazard**: `%s` reads unbounded characters until whitespace, easily overflowing destination buffers. **Must always specify width** (e.g., `%63s`).
2. **Whitespace Desynchronization**: If input contains unexpected formatting or extra spaces, `fscanf()` leaves characters in the file stream buffer, resulting in an infinite loop on subsequent reads.
3. **Leading Whitespace Behavior**: Most format specifiers skip leading whitespace automatically, but `%c` and `%[...]` (scansets) **do not** unless preceded by an explicit space in the format string (e.g., `" %c"`).

### The Gold Standard Production Pattern: `fgets()` + `sscanf()`
Always read an entire physical line safely into an in-memory string buffer with `fgets()`, then parse fields from memory with `sscanf()`.

```
Disk File Stream
       |
       v (fgets: Guarantees bounded line read up to buffer size)
[ Stack Line Buffer: "101,Ada Lovelace,94.5\n" ]
       |
       v (sscanf: Parses without stream desynchronization)
[ emp.id = 101 ] [ emp.name = "Ada Lovelace" ] [ emp.score = 94.5 ]
```

---

## 4. Side-by-Side Comparison: Parsing Techniques

| Feature | Direct `fscanf(fp, ...)` | `fgets()` + `sscanf()` | `fgetc()` / State Machine |
| :--- | :--- | :--- | :--- |
| **Buffer Safety** | Risk of overflow without width limits | Fixed buffer size guaranteed | 100% fine-grained control |
| **Error Recovery**| Hard to skip malformed lines | Trivial (read next line) | Resilient to partial tokens |
| **Trailing Whitespace** | Stream cursor remains mid-line | Consumes trailing newline | Handled manually |
| **Performance** | Direct parsing | Extra copy to buffer | Character-by-character overhead |
| **Production Recommendation** | Avoid for untrusted files | **Recommended for line formats** | Recommended for custom grammar |

---

## 5. Under the Hood: Format Parsing Engines

When `sscanf()` executes:
1. It iterates through the format string character by character.
2. Literal characters in the format string must match input characters exactly.
3. Upon hitting `%`, it parses the specifier and scans input characters using character classification functions (`isdigit()`, `isspace()`).
4. Converts numeric string representations to binary (e.g., internally invoking logic similar to `strtol()` / `strtod()`).
5. Returns the exact count of successfully matched and assigned input items.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Ignoring Return Value of `sscanf()` / `fscanf()`**:
   Assuming all fields matched when only a partial match occurred leads to using uninitialized local variables.
2. **Passing Variables instead of Pointers**:
   Writing `sscanf(buf, "%d", val)` instead of `&val` writes to the memory address represented by `val`'s integer value (Instant SIGSEGV).
3. **Format String Vulnerabilities**:
   Passing user input directly as the format string `fprintf(fp, user_input)` allows format string attacks. Always use `fprintf(fp, "%s", user_input)`.

---

## 7. Complexity & Production Best Practices

- **Time Complexity**: $O(N)$ where $N$ is the number of characters in the line.
- **Always Validate Match Count**:
  ```c
  if (sscanf(line, "%d,%63[^,],%lf", &id, name, &score) != 3) {
      /* Handle malformed row */
  }
  ```
- **Use Field Width Limits**: Always enforce `%63s` for a `char str[64]` buffer.
