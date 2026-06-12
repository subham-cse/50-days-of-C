# Day 06: Standard I/O, Streams, Format Specifiers, and Escape Sequences

## 1. Executive Overview & Mental Model
Standard Input/Output in C is abstracted around **Streams**—continuous byte conduits connecting the user program to operating system file descriptors, virtual terminals (TTYs), sockets, or disk files. The `<stdio.h>` library provides buffered I/O abstractions (`FILE *`) designed to minimize expensive kernel context switches by aggregating individual byte reads and writes into user-space memory buffers.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Cargo Shipping Ports vs Courier Vans               |
| - Unbuffered I/O (`_IONBF`) is dispatching an entire freight truck     |
|   for a single envelope (one system call per character: slow).         |
| - Buffered I/O (`_IOFBF` / `_IOLBF`) is packing an entire shipping     |
|   container before calling the crane (high-throughput batching).      |
+------------------------------------------------------------------------+
```

---

## 2. Standard Streams & Operating System File Descriptors

```
+------------------------------------------------------------------------+
| Process Virtual Environment                                            |
|                                                                        |
| [ FILE *stdin  ] <=== fd 0 <=== Keyboard / Input Pipe                  |
| [ FILE *stdout ] ===> fd 1 ===> Terminal Screen / Redirected File (>)  |
| [ FILE *stderr ] ===> fd 2 ===> Terminal Screen (Unbuffered Error Log) |
+------------------------------------------------------------------------+
```

### Stream Buffering Modes (`setvbuf`):
1. **Fully Buffered (`_IOFBF`):** Characters are written to the kernel only when the buffer is completely full (Default for files/pipes).
2. **Line Buffered (`_IOLBF`):** Characters are flushed to the OS whenever a newline `\n` is encountered (Default for interactive terminal stdout).
3. **Unbuffered (`_IONBF`):** Characters are dispatched immediately via `write()` syscall (Default for `stderr`).

---

## 3. Comprehensive Format Specifier Grammar
A format specifier follows the formal syntax:
$$\%[\text{flags}][\text{width}][.\text{precision}][\text{length}]\text{specifier}$$

```
+-------------------------------------------------------------------------------+
| Specifier | Expected Argument Type | Output Representation                    |
| :---      | :---                   | :---                                     |
| `%d` / `%i`| `int`                  | Signed decimal integer                   |
| `%u`      | `unsigned int`         | Unsigned decimal integer                 |
| `%o`      | `unsigned int`         | Unsigned octal integer                   |
| `%x` / `%X`| `unsigned int`        | Unsigned hexadecimal (lowercase / UPPER) |
| `%f` / `%F`| `double`               | Decimal floating-point notation          |
| `%e` / `%E`| `double`               | Scientific exponential notation          |
| `%g` / `%G`| `double`               | Shortest representation of `%f` or `%e`  |
| `%c`      | `int` (promoted char)  | Single ASCII/byte character              |
| `%s`      | `char *` (null-term)   | Null-terminated character string         |
| `%p`      | `void *`               | Implementation-defined memory address    |
| `%zu`     | `size_t`               | Unsigned object size integer (C99)       |
| `%zd`     | `ssize_t` / `ptrdiff_t`| Signed size or pointer difference (C99)  |
| `%%`      | None                   | Literal '%' character                    |
+-------------------------------------------------------------------------------+
```

### Length Sub-Specifiers:
- `hh` : `char` / `unsigned char`
- `h`  : `short` / `unsigned short`
- `l`  : `long` / `unsigned long` / `wchar_t`
- `ll` : `long long` / `unsigned long long`
- `L`  : `long double`

---

## 4. Escape Sequences Taxonomy

| Escape | Name | ASCII Dec / Hex | Control Character Purpose |
| :--- | :--- | :--- | :--- |
| `\n` | Line Feed (Newline) | 10 (0x0A) | Moves cursor to the start of the next line. Flushes `_IOLBF`. |
| `\r` | Carriage Return | 13 (0x0D) | Moves cursor to the absolute beginning of the current line. |
| `\t` | Horizontal Tab | 9 (0x09) | Advances cursor to the next horizontal tab stop (typically 8 cols). |
| `\b` | Backspace | 8 (0x08) | Moves cursor back one position (does not delete by itself). |
| `\a` | Alert (Bell) | 7 (0x07) | Triggers terminal acoustic beep or visual flash. |
| `\\` | Literal Backslash | 92 (0x5C) | Emits single backslash character. |
| `\"` | Double Quote | 34 (0x22) | Escapes string literal boundary inside `"..."`. |
| `\0` | Null Byte | 0 (0x00) | String terminator sentinel. |
| `\xHH` | Hex Byte | e.g. `\x1B` | Emits raw byte with hexadecimal value (e.g. ANSI escape code). |

---

## 5. Under the Hood: Format String Engines & Security Vulnerabilities

```
Vulnerable Code Pattern:
  char user_input[128];
  fgets(user_input, sizeof(user_input), stdin);
  printf(user_input);  // CRITICAL VULNERABILITY (CWE-134)
```

```
Stack State During Format String Attack:
+---------------------------------------------------------+
| [printf() Activation Frame]                             |
| Expected 1st arg: Format string pointer ("%08x %08x %n")|
| Expected 2nd arg: NOT SUPPLIED BY CALLER!               |
+---------------------------------------------------------+
| [Stack Memory of Caller]                                |
| 0x7FFF0018: Secret Auth Token / Return Address (Leaked!)|
| 0x7FFF0010: Saved Frame Pointer (%rbp) (Overwritable!)  |
+---------------------------------------------------------+
- The `%x` specifier forces `printf` to walk up the stack, leaking sensitive memory.
- The `%n` specifier writes the count of characters emitted so far into the target address!
```

---

## 6. Common Pitfalls & Safer Alternatives

| Insecure / Buggy Pattern | Root Cause | Robust Replacement |
| :--- | :--- | :--- |
| `scanf("%s", buffer);` | Unbounded input causes fatal buffer overflow. | `scanf("%127s", buffer);` or `fgets(buffer, sizeof(buffer), stdin);` |
| `printf(untrusted_str);` | Format string vulnerability / memory corruption. | `printf("%s", untrusted_str);` or `fputs(untrusted_str, stdout);` |
| Missing `fflush(stdout);` before crash | Buffered data lost in userspace buffer if process aborts. | Explicit `fflush(stdout);` or use unbuffered `stderr`. |
| `%d` with `size_t` | Type mismatch UB on 64-bit platforms (4-byte vs 8-byte stack slot). | Use `%zu` for all `size_t` variables. |

---

## 7. Complexity & Best Practices
- Never use `gets()` (removed from standard in C11).
- Always validate `scanf()` return values (number of successfully assigned items).
- Use `snprintf()` instead of `sprintf()` to guarantee bounds safety.
