# Day 50: Preprocessor Directives, Macros, and `#define` vs. `const`

## 1. Overview & Real-World Analogy
The **C Preprocessor (CPP)** is a textual transformation pass that runs before actual lexical analysis and compilation. Preprocessor directives (lines starting with `#`) perform file inclusion (`#include`), conditional compilation (`#if`, `#ifdef`), and macro expansion (`#define`).

### Real-World Analogy
Think of a book publishing workflow:
- **Preprocessor (`#define`, `#include`)**: The editor's text replacement script. It finds all occurrences of `[AUTHOR_NAME]` and replaces them with `"Jane Doe"` across all draft chapters before the printer begins. It has zero knowledge of grammar, syntax, or meaning.
- **Compiler (`const int X = 10`)**: The lead typesetter and proofreader who understands grammar, sentence structure, data types, and semantic rules.

```
Source Code (.c / .h)
        |
        v [ Preprocessor Pass (cpp / gcc -E) ] -> Macro Expansion, Header Ingestion
Preprocessed Translation Unit (.i)
        |
        v [ Compiler (cc1) ] -> Lexical, Syntax, Type Check, Optimization
Assembly Code (.s)
        |
        v [ Assembler (as) ]
Object Code (.o)
        |
        v [ Linker (ld) ] -> Symbol Resolution
Executable Binary (a.out / .exe)
```

---

## 2. Deep Technical Dive: Preprocessor Mechanics

### 1. Macro Token Substitution & Parenthesization Rule
Function-like macros perform raw token substitution. Failing to parenthesize macro parameters or the macro body leads to catastrophic operator precedence bugs:

```c
// BROKEN:
#define SQUARE(x) x * x
int result = SQUARE(2 + 3); // Expands to: 2 + 3 * 2 + 3 = 2 + 6 + 3 = 11 (Expected 25!)

// CORRECT:
#define SQUARE(x) ((x) * (x))
int result = SQUARE(2 + 3); // Expands to: ((2 + 3) * (2 + 3)) = 25
```

### 2. Side-Effect Hazards in Macro Arguments
Even parenthesized macros evaluate arguments multiple times if the parameter appears multiple times in the body:
```c
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
int x = 5, y = 10;
int m = MAX(x++, y++); // y++ is evaluated TWICE! y becomes 12!
```

### 3. Stringification (`#`) and Token Concatenation (`##`)
- `#parameter`: Converts a macro parameter into a string literal.
- `token1 ## token2`: Pastes two tokens together into a single identifier at compile time.

```c
#define PRINT_VAR(var) printf(#var " = %d\n", var)
#define MAKE_FN(name) void handle_##name(void)
```

### 4. The Multi-Statement Macro Idiom: `do { ... } while (0)`
When defining multi-statement macros, wrapping them in `{}` breaks `if-else` statements. The standard idiom is `do { ... } while (0)`:

```c
#define LOG_ACTION(msg) do {    \
    printf("[LOG] %s\n", msg);   \
    log_counter++;               \
} while (0)
```

---

## 3. Side-by-Side Comparison: `#define` vs. `const`

| Characteristic | `#define MAX_SIZE 1024` | `static const int MaxSize = 1024;` |
| :--- | :--- | :--- |
| **Processing Stage** | Preprocessor (Text substitution) | Compiler (Lexical / Semantic pass) |
| **Type Safety** | None (Untyped tokens) | Strongly typed (`int`, `size_t`, etc.) |
| **Scope** | Global from definition to EOF | Lexical / Block / File scope |
| **Debugging / Symbols**| Erased from debug symbol tables | Visible in debugger (`gdb`, `lldb`) |
| **Memory Allocation** | Zero (Inlined into instructions) | Stored in `.rodata` or constant-folded |
| **Array Sizing (C89/99)**| Constant expression (ICE) | Variable in C99 (VLA) unless enum |
| **Addressable?** | No (`&MAX_SIZE` is illegal) | Yes (`&MaxSize` produces valid pointer)|

---

## 4. Conditional Compilation & Include Guards

Header files must use include guards to prevent duplicate type declarations when included transitively:

```c
#ifndef MY_MODULE_H
#define MY_MODULE_H

/* Module declarations */

#endif /* MY_MODULE_H */
```

### Modern `#pragma once`
Supported by virtually all modern compilers (GCC, Clang, MSVC) as a high-performance alternative to manual header guards.

---

## 5. Under the Hood: The Preprocessor Pipeline

During the preprocessing pass:
1. **Trigraph & Backslash-Newline Splicing**: Physical lines ending with `\` are joined.
2. **Comment Stripping**: Comments (`/* ... */` and `// ...`) are replaced with a single space.
3. **Tokenization**: Stream divided into preprocessing tokens.
4. **Directive Execution**: `#include` physically pastes target files; `#ifdef` prunes dead code branches.
5. **Macro Expansion**: Recursive macro replacement without re-evaluating completed expansions.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Semicolon at End of `#define`**:
   `#define SIZE 100;` $\implies$ `int arr[SIZE];` expands to `int arr[100;];` (Syntax error).
2. **Recursive Macro Expansion**: Macros cannot call themselves recursively.
3. **Header Pollution**: Defining short macro names (e.g. `#define READ 1`) in header files pollutes the global namespace.

---

## 7. Complexity & Production Best Practices

- **Zero Runtime Overhead**: Preprocessor transformations occur entirely at compile time ($O(0)$ runtime time/space).
- **Production Standard**: Use `static const` or `enum` for typed constants; use `#define` exclusively for conditional flags, build configurations, and include guards.
