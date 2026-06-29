# Day 23: Strings Basics and I/O in C

## 1. Overview and Mental Model
In the C programming language, there is no primitive `string` data type (unlike C++, Java, or Python). Instead, a string is defined purely by convention:
> **A C string is a contiguous sequence of characters (`char`) terminated by a null byte (`'\0'`, ASCII value `0`).**

Without the terminating null character (`'\0'`), a character sequence is merely a raw byte array, not a C string. Functions across the standard library (e.g., `printf("%s")`, `strlen()`, `strcpy()`) iterate through character addresses until they encounter a byte equal to `0`.

### Real-World Analogy
Think of a string as a freight train with boxcars carrying individual letters. The final car is a specialized red caboose (`'\0'`). The train engineer (the C runtime) counts cars and processes cargo starting from the locomotive until spotting the caboose. If someone forgets to couple the caboose, the engineer keeps driving down the track into random unmapped terrain (memory out-of-bounds / segmentation fault).

---

## 2. Memory Layout & String Literals

### Character Array vs String Literal
There is a critical architectural distinction between a modifiable character array and an immutable string literal pointer.

```c
char arr[] = "Hello";        // Stored on the Stack (Read/Write)
char *ptr  = "Hello";        // Stored in the .rodata / Read-Only Text Segment
```

```text
Stack Buffer (char arr[] = "Hello"):
+-----+-----+-----+-----+-----+------+
| 'H' | 'e' | 'l' | 'l' | 'o' | '\0' |   (Length = 5, Size = 6 bytes)
+-----+-----+-----+-----+-----+------+
 0x7FFE00 ... (Stack Frame - Fully Modifiable)

Read-Only Data Segment (char *ptr = "Hello"):
Stack Pointer: [ ptr ] ------> +-----+-----+-----+-----+-----+------+
                               | 'H' | 'e' | 'l' | 'l' | 'o' | '\0' |
                               +-----+-----+-----+-----+-----+------+
                                0x00405000 (.rodata - Modifying causes SIGSEGV!)
```

---

## 3. String Input / Output: Functions & Security

### Why `gets()` is Banned (ISO C11 Deprecation)
`gets(buffer)` has no parameter for buffer capacity. It continuously reads from `stdin` until a newline or EOF is reached. If the input exceeds the array bounds, it smashes the stack frame, overwriting the function's return address—creating critical buffer-overflow vulnerabilities (e.g., Morris Worm, arbitrary code execution exploits).

### Safe Input: `fgets()`
`fgets(char *str, int size, FILE *stream)` reads at most `size - 1` characters from the stream and guarantees null-termination by placing `'\0'` after the last character read.

```c
char buffer[64];
if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Note: fgets retains the newline '\n' character if it fits in the buffer!
    // Trimming the newline:
    buffer[strcspn(buffer, "\n")] = '\0';
}
```

### Formatted I/O: `scanf()` with Width Specifiers
Using `scanf("%s", buffer)` is just as dangerous as `gets()`. Always supply a maximum field width matching `buffer_capacity - 1`:
```c
char name[32];
scanf("%31s", name); // Reads at most 31 characters + appends '\0'
```

---

## 4. Comparison Table: String Input Methods

| Function | Signature | Bounds Protected? | Reads Spaces? | Captures `\n`? | Safe for Production? |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`gets()`** | `char *gets(char *s)` | ❌ NO (Unbounded) | ✅ Yes | ❌ Stripped | 🚫 BANNED (Removed C11) |
| **`scanf("%s")`** | `int scanf("%s", s)` | ❌ NO | ❌ Stops at space | ❌ No | ⚠️ Unsafe without width |
| **`scanf("%31s")`**| `int scanf("%31s", s)`| ✅ YES (31 chars) | ❌ Stops at space | ❌ No | ✅ Safe for single tokens |
| **`fgets()`** | `char *fgets(s, n, f)`| ✅ YES (`n - 1` chars) | ✅ Yes | ✅ Yes (retains `\n`) | 🛡️ Recommended Standard |
| **`getline()`** | `ssize_t getline(...)`| ✅ YES (Auto-allocates)| ✅ Yes | ✅ Yes | 🛡️ POSIX.1-2008 Standard |

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Off-by-One Missing Terminator Allocation:**
   Allocating `char buf[5] = "Hello";` lacks room for `'\0'`. Any string function passing `buf` will read adjacent garbage memory until a random zero byte is encountered.
2. **Writing to String Literal via Pointer:**
   ```c
   char *str = "Read only text";
   str[0] = 'W'; // CRASH: SIGSEGV / Bus Error (Writing to Read-Only Page)
   ```
3. **Dangling Pointer to Local Stack String:**
   Returning `char*` pointing to a local array `char temp[64];` from a function returns a pointer to an invalidated stack frame.
4. **Failure to Strip Trailing Newline in `fgets`:**
   Comparing a string from `fgets` against `"exit"` will fail because `fgets` stores `"exit\n\0"`.

---

## 6. Production Best Practices & Complexity

- **Time Complexity:** String length and scanning operations are $\mathcal{O}(N)$ where $N$ is the number of characters before `'\0'`.
- **Space Complexity:** $\mathcal{O}(N + 1)$ bytes including null terminator.
- Always allocate `strlen(str) + 1` bytes for dynamic copies.
- Standardize on `fgets()` followed by a clean newline stripping idiom using `strcspn(buf, "\r\n") = '\0'`.
