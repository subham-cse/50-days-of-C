# Day 26: User-Defined Functions in C

## 1. Overview and Mental Model
A function in C is a self-contained block of reusable code designed to perform a specific, well-defined task. In structured programming, functions provide **procedural abstraction** and **modular decomposition**: breaking complex systems into testable, maintainable, and isolated logic units.

### Real-World Analogy
Consider an automobile engine with an alternator. The engine control unit does not need to know the electromagnetic winding physics inside the alternator; it simply sends mechanical rotation (input arguments) and receives regulated electrical voltage (return value). The alternator encapsulates its internal state and mechanics.

---

## 2. Anatomy of a C Function

A complete function lifecycle in C consists of three distinct elements:

```c
// 1. Function Declaration / Prototype
// Informs compiler of name, return type, and parameter types before invocation.
int calculate_power(int base, int exponent);

int main(void) {
    // 2. Function Call / Invocation
    // Passes actual arguments and captures returned value.
    int result = calculate_power(2, 8);
    return 0;
}

// 3. Function Definition
// The actual executable body of the function.
int calculate_power(int base, int exponent) {
    int total = 1;
    for (int i = 0; i < exponent; ++i) {
        total *= base;
    }
    return total;
}
```

---

## 3. The Call Stack & Activation Records ("Under the Hood")

When a function is called, the CPU and OS runtime allocate a slice of memory on the thread's **Call Stack** known as a **Stack Frame** (or **Activation Record**).

### Stack Frame Structure (x86-64 / ARM64 ABI)
```text
High Memory
+------------------------------------+
| Caller's Stack Frame               |
| - Caller Local Variables           |
| - Arguments pushed to stack (>6)   |
+------------------------------------+
| Return Address (RIP/PC)            |  <-- Saved instruction pointer for caller
+------------------------------------+
| Saved Frame Pointer (RBP / FP)     |  <-- Points to previous base frame
+------------------------------------+
| Callee Local Variables & Spills    |  <-- int total, int i
+------------------------------------+
| Callee Temporary Registers         |
+------------------------------------+  <-- Stack Pointer (RSP / SP)
Low Memory
```

### The Call/Return Sequence:
1. **Prologue:**
   - Caller places first arguments into CPU registers (`RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9` on x86-64 System V ABI).
   - CPU executes `call` instruction: pushes Return Address onto the stack and jumps to callee address.
   - Callee saves caller's frame pointer (`push rbp; mov rbp, rsp`) and decrements stack pointer to allocate local variables.
2. **Epilogue:**
   - Callee writes return value into register `RAX` (or `EAX`).
   - Callee restores stack pointer and caller frame pointer (`mov rsp, rbp; pop rbp` or `leave`).
   - CPU executes `ret` instruction: pops Return Address from stack into Instruction Pointer (`RIP`).

---

## 4. Function Categories in C

| Category | Return Type | Parameters | Typical Use Case |
| :--- | :--- | :--- | :--- |
| **Pure Computation** | Non-void (`int`, `double`) | `(int a, int b)` | Math calculations, hashing, transformations |
| **State Mutator** | `void` or `bool` | `(T *dest, ...)` | In-place modifications, hardware register writes |
| **Consumer / Sink** | `void` | `(const char *msg)` | Logging, printing, telemetry export |
| **Generator / Source** | Non-void | `(void)` | `rand()`, `getchar()`, timestamp getters |

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Implicit Function Declarations (Banned in C99/C11):**
   In legacy C89, calling an undeclared function assumed it returned `int`. In C99 and later, this is an error or undefined behavior if the actual return type is a pointer or floating-point type.
2. **Missing Return Statement in Non-Void Function:**
   Falling off the end of a non-void function without a `return` statement yields undefined behavior (the caller reads whatever arbitrary garbage was left in the `RAX` register).
3. **Returning Pointers to Local Automatic Stack Variables:**
   ```c
   int* bad_function(void) {
       int x = 42;
       return &x; // UB: Stack frame deallocated on return!
   }
   ```
4. **Parameter Mismatches:**
   Passing mismatched argument types without prototypes can lead to corrupted register passing and stack misalignment.

---

## 6. Production Best Practices & Inline Optimization

- **Single Responsibility Principle:** Each function should do one cohesive task.
- **`static` Scope for Local Helpers:** Mark internal functions `static` to restrict linkage to the current translation unit (`.c` file) and enable aggressive compiler optimizations.
- **`inline` Keyword:** Suggests compiler to expand the function body at call sites to eliminate call/ret prologue/epilogue overhead for tiny, hot-path routines.
- **Const Correctness:** Use `const` on pointer parameters that are only read: `void log_msg(const char *msg)`.
