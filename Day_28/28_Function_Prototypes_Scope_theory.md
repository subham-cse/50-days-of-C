# Day 28: Function Prototypes, Variable Scope, and Storage Classes in C

## 1. Overview and Fundamental Principles
In C, the visibility and lifespan of any identifier (function or variable) are governed by three complementary concepts:
1. **Scope**: Where in the source code an identifier is visible and accessible.
2. **Linkage**: How an identifier relates across multiple translation units (`.c` files).
3. **Storage Duration (Lifetime)**: When memory for a variable is allocated and deallocated by the runtime.

---

## 2. Deep Technical Breakdown: Scope, Linkage, and Lifetime

```text
+-------------------+--------------------------------------------------------------------------+
| Scope Category    | Description & Accessibility                                              |
+-------------------+--------------------------------------------------------------------------+
| Block Scope       | Defined within { ... }. Visible only within enclosing braces.           |
| File Scope        | Defined outside all functions. Visible from declaration to end of file.  |
| Function Scope    | Applies ONLY to goto labels (visible throughout the entire function).    |
| Prototype Scope   | Parameter names in function prototypes (discarded after semicolon).      |
+-------------------+--------------------------------------------------------------------------+
```

### The Four Storage Classes in C

```c
// 1. auto (Default for local variables)
// Allocated on Stack Frame; destroyed upon block exit.
auto int local_var = 5; 

// 2. static
// Allocated in .data or .bss section; persists for entire program lifetime.
// Local static: Retains value between function invocations.
// Global static: Restricts linkage to the current translation unit (file-private).
static int call_counter = 0;

// 3. extern
// Declares an identifier defined in another translation unit without allocating new storage.
extern int global_system_clock;

// 4. register
// Hint to the compiler to store variable directly in a CPU register (e.g. ECX) for speed.
// Note: Cannot take address (&reg_var) because registers do not have memory addresses!
register int fast_counter = 0;
```

---

## 3. Function Prototypes & Forward Declarations

A function prototype declares the function's signature before its actual definition is encountered.

### Why Prototypes are Mandatory in Modern C
1. **Argument Type Checking:** Enables the compiler to perform automatic implicit conversions (e.g., passing `int 5` to a function expecting `double 5.0`) or emit warnings on mismatched pointer types.
2. **Mutual Recursion Support:** Allows Function `A` to call Function `B` while Function `B` calls Function `A`.
3. **ABI Parameter Passing:** Dictates whether parameters are loaded into integer registers (`RDI`, `RSI`) or floating-point registers (`XMM0`, `XMM1`).

```text
Compilation Flow:
[ header.h ] (Prototypes: void func(double);)
     |
     v
[ source.c ] (Compiler knows func takes XMM0 register BEFORE generating call instruction)
```

---

## 4. Storage Class Summary Matrix

| Specifier | Storage Location | Scope | Lifetime | Linkage | Initialized Default |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`auto`** | Stack Frame | Block | Block Execution | None | Garbage values |
| **`static` (local)** | `.data` / `.bss` | Block | Program Lifetime | None | Zero (`0`) |
| **`static` (global)**| `.data` / `.bss` | File | Program Lifetime | Internal (Private) | Zero (`0`) |
| **`extern`** | `.data` / `.bss` | File | Program Lifetime | External (Public) | Determined by definition |
| **`register`** | CPU Register / Stack | Block | Block Execution | None | Garbage values |

---

## 5. Shadowing (Variable Masking)

When a variable declared within an inner block shares the same name as a variable in an outer scope, the inner variable **shadows** the outer variable.

```c
int x = 50; // Global File Scope
void test(void) {
    int x = 20; // Shadows global x
    {
        int x = 10; // Shadows local x
        printf("%d\n", x); // Prints 10
    }
    printf("%d\n", x); // Prints 20
}
```

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Address-of on Register Variable:**
   Executing `&var` when `register int var;` is used results in a compile-time error (`error: address of register variable requested`).
2. **Missing `extern` in Header Files:**
   Defining `int global_var = 10;` in a `.h` file included by multiple `.c` files causes a linker error: `multiple definition of 'global_var'`. The header must contain `extern int global_var;` and exactly one `.c` file must contain `int global_var = 10;`.
3. **Static Lifetime Re-initialization Myth:**
   `static int count = 0;` inside a function is evaluated **only once at program load time**, not upon each function entry.
4. **Shadowing Bugs:**
   Accidentally declaring a local variable with the same name as a global configuration parameter leads to silent logic errors.

---

## 7. Production Best Practices

- Always write complete function prototypes in header files.
- Apply `static` to every function and file-scope variable that is not explicitly part of the public API.
- Use `static` local variables judiciously for thread-safe state caching or execution counters (beware of reentrancy issues in multi-threaded environments).
- Never use non-const global variables when data can be passed cleanly via parameter pointers.
