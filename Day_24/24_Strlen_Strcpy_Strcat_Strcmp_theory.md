# Day 24: Standard String Functions (`strlen`, `strcpy`, `strcat`, `strcmp`) and Safe Alternatives

## 1. Overview and Mental Model
The C standard library header `<string.h>` provides fundamental primitives for manipulating null-terminated strings. These routines operate by dereferencing pointers sequentially until the null sentinel byte (`'\0'`) is discovered.

Understanding the internal mechanics, algorithmic complexity, and vulnerability attack surfaces of these four functions is a cornerstone of robust systems programming.

---

## 2. Deep Technical Breakdown & Custom Implementations

### 1. `strlen()` (String Length)
- **Role:** Counts the number of characters in a string **excluding** the terminating null byte.
- **Mental Model:** A walking pointer increments until `*ptr == '\0'`.
```c
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p != '\0') {
        p++;
    }
    return (size_t)(p - s);
}
```

### 2. `strcpy()` & `strncpy()` (String Copy)
- **Role:** Copies bytes from source (`src`) to destination (`dest`), including the null terminator.
- **Security Vulnerability:** `strcpy()` does not check if `dest` is large enough. If `src` length exceeds `dest` buffer capacity, it causes a stack buffer overflow.
- **`strncpy()` Caveat:** If `src` has $\ge n$ characters, `strncpy` does **NOT** append a null terminator!
```c
char *my_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0') {
        // Copy until null character is copied
    }
    return dest;
}
```

### 3. `strcat()` & `strncat()` (String Concatenation)
- **Role:** Appends `src` to the end of `dest`, overwriting `dest`'s initial null terminator and adding a new null terminator at the end.
- **Complexity Trap:** `strcat` must scan through `dest` from index 0 to find `'\0'` before appending. Repeatedly calling `strcat` in a loop produces quadratic $\mathcal{O}(N^2)$ running time ("Schlemiel the Painter's Algorithm").
```c
char *my_strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d != '\0') d++; // Advance to the null terminator
    while ((*d++ = *src++) != '\0') ; // Append source
    return dest;
}
```

### 4. `strcmp()` & `strncmp()` (String Comparison)
- **Role:** Performs lexicographical (ASCII order) comparison between two strings.
- **Return Value Semantics:**
  - `< 0` : `s1` is lexicographically less than `s2` (first mismatch `(unsigned char)*s1 < (unsigned char)*s2`).
  - `== 0`: `s1` and `s2` are identical.
  - `> 0` : `s1` is lexicographically greater than `s2`.
```c
int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (int)((unsigned char)*s1 - (unsigned char)*s2);
}
```

---

## 3. Memory Progression in String Concatenation

```text
Initial Destination:   "Hello\0"   (Allocated buffer: 16 bytes)
Initial Source:        " World\0"

Step 1: Locate '\0' in dest
[ H ][ e ][ l ][ l ][ o ][\0][ ? ][ ? ][ ? ][ ? ][ ? ][ ? ][ ? ][ ? ][ ? ][ ? ]
                          ^ Pointer d stops here

Step 2: Copy characters from src overwriting '\0'
[ H ][ e ][ l ][ l ][ o ][   ][ W ][ o ][ r ][ l ][ d ][\0][ ? ][ ? ][ ? ][ ? ]
                                                         ^ New terminator placed
```

---

## 4. Modern & Safe Alternatives

| Legacy Function | Inherent Danger | Modern Standard / Idiomatic Replacement | Why It Is Safer |
| :--- | :--- | :--- | :--- |
| **`strcpy(dst, src)`** | Unbounded stack smash | `snprintf(dst, sizeof(dst), "%s", src)` or BSD `strlcpy` | Always guarantees null-termination and enforces bounds |
| **`strcat(dst, src)`** | Unbounded stack smash + $\mathcal{O}(N^2)$ | `snprintf(dst + len, sizeof(dst) - len, "%s", src)` | Bounds-checked with explicit offset |
| **`strcmp(s1, s2)`** | Unbounded read if unterminated | `strncmp(s1, s2, max_len)` | Limits maximum character comparisons |
| **`strncpy(dst, src, n)`**| Does not null-terminate if truncated | `snprintf` / explicit manual `dst[n-1] = '\0'` | Avoids trailing read corruptions |

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Overlapping Buffers:**
   Calling `strcpy(str + 2, str)` is undefined behavior. For overlapping memory regions, `memmove()` must be used.
2. **Signed Char Subtraction in `strcmp`:**
   Failing to cast characters to `unsigned char` before subtraction leads to implementation-defined results when processing extended ASCII / UTF-8 byte sequences.
3. **Schlemiel the Painter Loop Overhead:**
   Doing `strcat(buffer, chunk)` 10,000 times re-scans the growing buffer every single iteration, degrading throughput from $\mathcal{O}(N)$ to $\mathcal{O}(N^2)$.

---

## 6. Complexity Summary

- **`strlen`**: $\mathcal{O}(N)$ time, $\mathcal{O}(1)$ space.
- **`strcpy`**: $\mathcal{O}(N)$ time, $\mathcal{O}(1)$ space.
- **`strcat`**: $\mathcal{O}(M + N)$ time, $\mathcal{O}(1)$ space (where $M = \text{len(dest)}$, $N = \text{len(src)}$).
- **`strcmp`**: $\mathcal{O}(\min(L_1, L_2))$ time, $\mathcal{O}(1)$ space.
