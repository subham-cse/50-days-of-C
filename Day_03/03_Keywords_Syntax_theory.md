# Day 03: Keywords, Lexical Grammar, and Syntax Taxonomy

## 1. Executive Overview & Mental Model
In the formal grammar of the C programming language, **Keywords** are reserved lexical tokens defined by the ISO C standard that possess immutable, pre-assigned syntactic and semantic meanings. The compiler's lexical scanner recognizes keywords during tokenization and disallows their use as user-defined identifiers (such as variable names, function names, or typedef tags).

```
+------------------------------------------------------------------------+
| Real-World Analogy: Legal Code and Traffic Signals                     |
| Keywords are the non-negotiable traffic signs ("STOP", "ONE WAY",      |
| "YIELD") of the language. You cannot name your street "STOP" because  |
| it would create grammatical ambiguity in traffic law. Similarly, you   |
| cannot name a variable `while` or `int` without breaking parser logic. |
+------------------------------------------------------------------------+
```

---

## 2. Comprehensive Keyword Evolution Across ISO Standards

```
+---------------------------------------------------------------------------+
| C89/C90 (32 Keywords)                                                     |
| auto     break    case     char     const    continue default  do         |
| double   else     enum     extern   float    for      goto     if         |
| int      long     register return   short    signed   sizeof   static     |
| struct   switch   typedef  union    unsigned void     volatile while      |
+---------------------------------------------------------------------------+
                                     |
                                     v (+5 in C99 = 37)
+---------------------------------------------------------------------------+
| C99 Additions:                                                            |
| inline   restrict _Bool    _Complex _Imaginary                            |
+---------------------------------------------------------------------------+
                                     |
                                     v (+7 in C11 = 44)
+---------------------------------------------------------------------------+
| C11 Additions:                                                            |
| _Alignas _Alignof _Atomic  _Generic _Noreturn _Static_assert _Thread_local|
+---------------------------------------------------------------------------+
                                     |
                                     v (Standardized in C23 = 48+)
+---------------------------------------------------------------------------+
| C23 Additions & Lowercase Aliases:                                        |
| alignas  alignof  bool     constexpr false   nullptr  static_assert       |
| thread_local true typeof   typeof_unqual                                  |
+---------------------------------------------------------------------------+
```

---

## 3. Categorized Keyword Taxonomy (ISO C99 / C11)

### A. Primitive Data Types & Modifiers
- **Base Types:** `void`, `char`, `int`, `float`, `double`, `_Bool` (via `<stdbool.h>` `bool`), `_Complex`, `_Imaginary`.
- **Size/Sign Modifiers:** `signed`, `unsigned`, `short`, `long` (including `long long`).

### B. Storage Class Specifiers
Dictate **storage duration**, **scope**, and **linkage**:
- `auto`: Default automatic storage duration for local variables inside block scope.
- `register`: Suggests caching the variable directly in a CPU register (address-of operator `&` forbidden).
- `static`: Internal linkage at file scope; static lifetime (retains value across calls) at block scope.
- `extern`: Declares a symbol with external linkage defined in another translation unit.
- `_Thread_local` (C11): Per-thread unique instance storage.

### C. Type Qualifiers
Provide immutability, hardware synchronization, and optimization hints:
- `const`: Read-only value qualifier; prevents compiler-allowed mutation.
- `volatile`: Informs the compiler that the value may mutate via external hardware/interrupts; inhibits register caching optimizations.
- `restrict` (C99): Promise to the compiler that the pointer is the sole alias for the referenced memory object, unlocking aggressive vectorization and reordering.

### D. Flow Control & Selection Statements
- `if`, `else`, `switch`, `case`, `default`.
- `for`, `while`, `do`.
- `goto`, `continue`, `break`, `return`.

### E. Compound Type Construction
- `struct`: Sequential contiguous memory layout of heterogeneous data fields.
- `union`: Overlaid shared memory location for heterogeneous data (size = max member).
- `enum`: User-defined enumerated integral constants.
- `typedef`: Aliases a type expression with a descriptive new name.

### F. Specialized Modern Specifiers
- `inline` (C99): Compiler optimization hint requesting function body expansion at call sites.
- `_Generic` (C11): Compile-time type-based expression dispatch (C's answer to lightweight polymorphism).
- `_Static_assert` (C11): Compile-time boolean validation with diagnostic messaging.
- `_Alignas` / `_Alignof` (C11): Custom memory boundary alignment specifications.

---

## 4. Syntax & Statement Grammar Anatomy

```
Statement Taxonomy:
===================
1. Labeled Statement:       case 10: | default: | error_handler:
2. Compound Statement:      { stmt1; stmt2; ... } (Introduces new block scope)
3. Expression Statement:    x = y + 42; | puts("Hello"); | ; (Null statement)
4. Selection Statement:     if (expr) stmt else stmt | switch (expr) stmt
5. Iteration Statement:     while (expr) stmt | for (init; cond; post) stmt
6. Jump Statement:          goto label; | continue; | break; | return expr;
```

---

## 5. Storage Duration and Linkage Matrix

| Storage Class | Storage Duration | Default Scope | Default Linkage | Memory Segment |
| :--- | :--- | :--- | :--- | :--- |
| `auto` | Automatic (Stack frame) | Block Scope | None | Stack |
| `register` | Automatic (Stack/Reg) | Block Scope | None | CPU Register / Stack |
| `static` (Block) | Static (Program lifetime) | Block Scope | None | `.data` / `.bss` |
| `static` (File) | Static (Program lifetime) | File Scope | Internal (Translation unit) | `.data` / `.bss` |
| `extern` | Static (Program lifetime) | File Scope | External (Cross-unit) | `.data` / `.bss` |

---

## 6. Under the Hood: Lexer & Compiler Code Generation

```
Source Code Text: "static volatile const int sensor_val = 100;"
                                 |
                                 v Lexical Scanner
Token Stream: [KEYWORD: static] [KEYWORD: volatile] [KEYWORD: const] [KEYWORD: int] 
              [IDENTIFIER: sensor_val] [OPERATOR: =] [LITERAL_INT: 100] [PUNCT: ;]
                                 |
                                 v Parser & Code Generator
Assembler Directives Emitted:
  .section .data,"aw",@progbits
  .align 4
  .type sensor_val, @object
  .size sensor_val, 4
sensor_val:
  .long 100
```
- **`volatile` Impact:** When reading `sensor_val`, the compiler issues a dedicated `movl sensor_val(%rip), %eax` instruction on *every single access*, bypassing register reuse optimizations.
- **`restrict` Impact:** Omits memory reload instructions between pointer writes, reducing pipeline stalls.

---

## 7. Common Pitfalls & Forbidden Syntax

| Pitfall | Code Example | Consequence | Correct Pattern |
| :--- | :--- | :--- | :--- |
| **Taking address of `register`** | `register int x; int *p = &x;` | Compile-time error: `address of register variable requested`. | Omit `register` if pointer reference is needed. |
| **Omitting `volatile` on MMIO / ISR flags** | `bool flag = false; while(!flag);` | Infinite loop: compiler optimizes read into register once. | `volatile bool flag = false;` |
| **Keyword Identifier Collision** | `int default = 5;` | Fatal syntax error during parsing. | Use domain-prefixed identifier: `int default_val = 5;`. |
| **`case` without integral constant** | `case my_variable:` | Compile error: `case label does not reduce to an integer constant`. | Must use compile-time integer constant or enum. |

---

## 8. Complexity & Best Practices
- Keep variable scope as narrow as possible (declare near first use in C99).
- Use `const` defensively on all pointer parameters that are read-only (`const char *str`).
- Use `_Static_assert` (or `static_assert`) to enforce platform structural invariant sizes (e.g., verifying 64-bit struct padding).
