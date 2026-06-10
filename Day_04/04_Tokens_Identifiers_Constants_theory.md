# Day 04: Tokens, Identifiers, Constants, and Literal Semantics

## 1. Executive Overview & Mental Model
At the lowest grammatical level, a C program is processed by the compiler's lexical scanner as a linear sequence of atomic building blocks known as **Tokens**. A token is the smallest lexical element that carries semantic meaning to the compiler. Whitespace (spaces, tabs, newlines) and comments serve as delimiters and are discarded during lexical analysis, except where they prevent token concatenation.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Written Human Language Lexicography                |
| - Characters = Alphabet letters                                        |
| - Tokens = Words, punctuation marks, and numbers                       |
| - Whitespace = Spacing between words                                   |
| - Grammar (Syntax) = Sentence structure rules (Subject-Verb-Object)    |
+------------------------------------------------------------------------+
```

---

## 2. The Six Token Classes in ANSI/ISO C

```
                     +-----------------------------------+
                     |       C Lexical Token Stream      |
                     +-----------------------------------+
                                       |
    +---------------+---------------+--+------------+---------------+---------------+
    |               |               |               |               |               |
    v               v               v               v               v               v
[Keywords]    [Identifiers]   [Constants]   [String Literals][Punctuators]     [Operators]
 `while`       `sensor_val`     `0x1F4`        `"Hello\n"`      `; { } ,`       `+ * -> ==`
```

### The Maximal Munch Rule (Longest Match Rule)
ISO C specifies that the lexical analyzer must construct the longest possible valid token from the remaining input stream.
- **Example:** `a+++b` is tokenized as `a` `++` `+` `b` (i.e., `(a++) + b`), never `a` `+` `++` `b`.
- **Trap:** `a+++++b` is tokenized as `a` `++` `++` `+` `b`, triggering a compile error because the second `++` attempts to operate on an rvalue expression `(a++)++`.

---

## 3. Identifiers & Name Resolution Rules
An identifier designates a variable, function, structure tag, typedef, or label.

### Formal Rules (ISO C99 §6.4.2):
1. Must begin with an ASCII letter (`a`-`z`, `A`-`Z`) or underscore (`_`).
2. Subsequent characters may include decimal digits (`0`-`9`), letters, or underscores.
3. Case-sensitive: `DataValue`, `datavalue`, and `DATAVALUE` are distinct identifiers.
4. **Significant Length Limits:**
   - C89: At least 31 characters for internal identifiers; 6 characters (case-insensitive) for external identifiers.
   - C99: At least 63 characters for internal identifiers; 31 characters (case-sensitive) for external identifiers.

### Reserved Identifier Namespace:
- Identifiers starting with double underscore `__` or an underscore followed by an uppercase letter `_A`-`_Z` are reserved for compiler implementations across all scopes.
- Identifiers starting with a single underscore `_` are reserved for file-scope declarations.

---

## 4. Taxonomy of Literals and Constants

### A. Integer Constants & Radix Representations
```
Format       Prefix    Example      Type Deductions (without suffix)
----------------------------------------------------------------------
Decimal      (None)    142          int -> long int -> long long int
Octal        0         0216 (=142)  int -> unsigned int -> long -> ...
Hexadecimal  0x / 0X   0x8E (=142)  int -> unsigned int -> long -> ...
Binary (C23) 0b / 0B   0b10001110   int -> unsigned int -> long -> ...
```

#### Integer Suffixes:
- `U` or `u`: `unsigned`
- `L` or `l`: `long`
- `LL` or `ll`: `long long`
- Combinations: `100ULL` (`unsigned long long`), `0x7FFFFFFFU` (`unsigned int`).

### B. Floating-Point Constants
- **Decimal Floating Constants:** `3.14159` (default type `double`), `3.14159f` / `3.14159F` (`float`), `3.14159L` (`long double`).
- **Scientific Exponent Notation:** `1.25e-3` ($1.25 \times 10^{-3} = 0.00125$).
- **Hexadecimal Floating Constants (C99):** `0x1.fp3` ($1 + 15/16 \times 2^3 = 1.9375 \times 8 = 15.5$).

### C. Character & String Literals
- **Character Literals (`char`):** Enclosed in single quotes `'A'` (ASCII 65). In C, character literals are typed as `int` (sizeof `'A'` == `sizeof(int)` == 4 bytes), unlike C++ where `sizeof('A') == 1`.
- **String Literals (`char[]`):** Enclosed in double quotes `"Embedded\0"`. Compiler appends implicit null terminator `\0`.
- **Automatic Concatenation:** `"Hello, " "World!"` is concatenated into a single string literal `"Hello, World!"` at compile time.

---

## 5. Memory Representation: String Literals vs. Character Arrays

```
Code Example:
  char *p_str = "Antigravity";      // Pointer to read-only string literal
  char  arr[] = "Antigravity";      // Stack-allocated mutable character array

Process Virtual Address Space:
+--------------------------------------------------------------------+
| .rodata Section (Read-Only Data Segment)                           |
| Address: 0x00405000: ['A']['n']['t']['i']['g']['r']['a']['v']['i']['t']['y']['\0']
|                         ^                                          |
|                         |                                          |
+-------------------------|------------------------------------------+
| Stack Frame             |                                          |
| p_str (Pointer: 8 bytes)| points to 0x00405000                     |
|                                                                    |
| arr[] (Local Array: 12 bytes on stack):                            |
| Address: 0x7FFF0010: ['A']['n']['t']['i']['g']['r']['a']['v']['i']['t']['y']['\0']
| (Copied from .rodata to stack at runtime; safe to mutate)          |
+--------------------------------------------------------------------+
```

---

## 6. Under the Hood: Lexer Finite Automata & Constant Folding
During the parsing phase, the compiler performs **Constant Folding**:
- Expressions involving pure compile-time literals (e.g., `uint32_t buffer_sz = 1024 * 64;`) are evaluated directly by the compiler frontend.
- No arithmetic instructions (`imul`) are generated in assembly; the value `65536` is emitted directly as an immediate operand (`$65536`).

```
AST Transformation:
      [*]  (Multiply Operator)            Emitted Assembly:
     /   \                                movl $65536, -4(%rbp)
 [1024] [64]  ==> [Constant: 65536]
```

---

## 7. Common Pitfalls & Dangerous Edge Cases

| Pitfall | Code Snippet | Root Cause & Hazard | Correct Alternative |
| :--- | :--- | :--- | :--- |
| **Accidental Octal Literals** | `int month = 09;` | Compile error: `9` is invalid digit in base-8 octal (`0` prefix). `08` and `09` are illegal. | `int month = 9;` |
| **Mutating String Literals** | `char *s = "read"; s[0]='b';` | Segmentation fault / UB: String literals reside in read-only `.rodata`. | `char s[] = "read"; s[0]='b';` |
| **Silent Integer Literal Overflow** | `int x = 3000000000;` | Exceeds 32-bit signed `int` range ($> 2^{31}-1$). | `int64_t x = 3000000000LL;` |
| **Multicharacter Constants** | `int val = 'AB';` | Implementation-defined value; non-portable endianness. | Use integer hex literal `0x4142` or byte array. |

---

## 8. Best Practices
1. Suffix numerical constants explicitly when targeting unsigned or extended-width types (`42ULL`, `3.141592653589793L`).
2. Never store string literal references in non-const `char *` pointers. Always write `const char *msg = "text";`.
3. Use enumeration constants (`enum`) or `static const` rather than magic numbers in algorithmic loops.
