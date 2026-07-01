# Day 25: String Reversal and Case Transformations in C

## 1. Overview and Mental Model
String transformations—such as in-place reversal, uppercase/lowercase mapping, title casing, and palindrome validation—are core string algorithms in text processing engines, compilers, and parsers.

### The Two-Pointer In-Place Reversal Paradigm
Rather than allocating a second string buffer, in-place reversal operates with two pointers (or array indices):
1. **Left pointer (`left`)**: Initialized to index `0` (start of the string).
2. **Right pointer (`right`)**: Initialized to index `len - 1` (last valid character before `'\0'`).
3. While `left < right`: Swap the characters at `left` and `right`, increment `left`, and decrement `right`.

```text
Initial String: "ROCKET"
Step 1: Swap str[0] and str[5]  -> [ T ][ O ][ C ][ K ][ E ][ R ]  (left=1, right=4)
Step 2: Swap str[1] and str[4]  -> [ T ][ E ][ C ][ K ][ O ][ R ]  (left=2, right=3)
Step 3: Swap str[2] and str[3]  -> [ T ][ E ][ K ][ C ][ O ][ R ]  (left=3, right=2: STOP)
Final: "TEKCOR"
```

---

## 2. ASCII Character Arithmetic & Bit Manipulation

The ASCII standard assigns contiguous integer values to the Latin alphabet:
- `'A'` to `'Z'`: 65 to 90 (Binary `01000001` to `01011010`)
- `'a'` to `'z'`: 97 to 122 (Binary `01100001` to `01111010`)

### The 32-Offset Rule (Bit 5 Flip)
Notice the difference between `'a'` (97) and `'A'` (65) is exactly $32 = 2^5$ (`0x20`).
- **To Lowercase:** `c | 0x20` (sets the 5th bit).
- **To Uppercase:** `c & ~0x20` or `c & 0xDF` (clears the 5th bit).
- **Toggle Case:** `c ^ 0x20` (flips the 5th bit).

```text
Character 'A': 0 1 0 0 0 0 0 1 (65)
Bitwise OR 32: 0 0 1 0 0 0 0 0 (32)
Result 'a':    0 1 1 0 0 0 0 1 (97)
```

---

## 3. Palindrome Checking: Symmetric String Verification

A string is a **palindrome** if it reads the same forwards and backwards (e.g., `"level"`, `"racecar"`, or alphanumeric case-insensitive phrases like `"A man, a plan, a canal: Panama"`).

### Algorithmic Strategy (Two-Pointer Skip Algorithm)
1. Initialize `left = 0`, `right = len - 1`.
2. Advance `left` past any non-alphanumeric punctuation.
3. Decrement `right` past any non-alphanumeric punctuation.
4. Compare `tolower(str[left])` with `tolower(str[right])`. If unequal, return `false`.
5. Repeat until `left >= right`.

---

## 4. Comparison Table: Transformation Techniques

| Algorithm | Method | Time Complexity | Auxiliary Space | Modifies Input? |
| :--- | :--- | :--- | :--- | :--- |
| **In-Place Reverse** | Two-Pointer Swap | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Yes |
| **Out-of-Place Reverse** | Copy to New Buffer | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | No |
| **Standard `toupper`/`tolower`** | `<ctype.h>` Iteration | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | Optional |
| **Bitwise Case Masking** | `c ^ 0x20` Bit Flip | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | In-place |
| **Palindrome Verification** | Two-Pointer Match | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | No |

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Attempting In-Place Mutation on Read-Only Literals:**
   ```c
   char *str = "radar";
   reverse_string(str); // CRASH: SIGSEGV / Bus Error (Writing to .rodata)
   ```
2. **Reversing Past the Null Terminator:**
   Starting `right` at `sizeof(buffer)` instead of `strlen(buffer) - 1` swaps the null terminator `'\0'` into the middle of the string, corrupting it into an empty string `""` on display.
3. **Locale-Dependent `ctype.h` Passing:**
   Passing signed `char` with negative values (e.g. extended ASCII) to `toupper(int c)` invokes undefined behavior. Always cast `toupper((unsigned char)str[i])`.
4. **UTF-8 Multi-byte Corruption:**
   Byte-swapping a UTF-8 string with multi-byte characters (e.g., emoji, Cyrillic) splits multi-byte code units, corrupting the Unicode stream.

---

## 6. Production Best Practices & Complexity

- Always use in-place two-pointer reversal whenever modifying the original buffer is acceptable to maintain $\mathcal{O}(1)$ space efficiency.
- For case conversions, favor `<ctype.h>` functions (`toupper`, `tolower`) over hardcoded ASCII subtraction to ensure standards compliance.
- Guard against null pointers (`if (!str) return;`).
