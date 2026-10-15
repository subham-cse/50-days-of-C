# Day 49: Domain Engine: Recursive-Descent AST Parser & Stack Bytecode VM

## 1. Title & Architectural Overview
At the pinnacle of systems programming lies the implementation of programming languages, domain-specific engines, and execution runtimes. Rather than relying on rigid string matching or third-party interpreters, production systems often compile expressions into an **Abstract Syntax Tree (AST)** and then lower that tree into compact **Bytecode Instructions** executed by a virtual processor—a **Bytecode Virtual Machine (VM)**.

### Real-World Analogy
Think of a high-efficiency automated manufacturing plant:
1. **Source Text**: The client's raw blueprint order written on paper.
2. **Lexer / Scanner**: An optical reader that cuts the blueprint into discrete standardized job cards (**Tokens** like numbers, arithmetic operators, parentheses).
3. **AST Parser**: An engineering scheduler that structures the job cards into a hierarchical dependency tree (**AST**) ensuring order of operations (welding before painting).
4. **Bytecode Compiler**: A translator converting the dependency tree into a linear stream of compact binary control codes (**Bytecode Instructions** with opcodes like `OP_PUSH`, `OP_ADD`, `OP_MUL`).
5. **Bytecode Virtual Machine (VM)**: An ultra-fast assembly line robot with an **Operand Stack** that executes instruction after instruction with zero overhead, yielding the finished physical product.

```
+-----------------------------------------------------------------------------------------+
|                                COMPILATION & RUNTIME PIPELINE                           |
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|  Source: "(10 + 20) * 3"                                                                |
|            |                                                                            |
|            v  [Lexer / Scanner]                                                         |
|  Tokens: [TOK_LPAREN, TOK_NUM(10), TOK_PLUS, TOK_NUM(20), TOK_RPAREN, TOK_STAR, ...]   |
|            |                                                                            |
|            v  [Recursive-Descent Parser]                                                |
|  AST:            BinaryOp ('*')                                                         |
|                 /              \                                                        |
|         BinaryOp ('+')       Number (3)                                                 |
|          /          \                                                                   |
|      Number (10)   Number (20)                                                          |
|            |                                                                            |
|            v  [Bytecode Emitter]                                                        |
|  Bytecode:  OP_PUSH const[0](10) -> OP_PUSH const[1](20) -> OP_ADD ->                   |
|             OP_PUSH const[2](3)  -> OP_MUL -> OP_PRINT -> OP_HALT                       |
|            |                                                                            |
|            v  [Virtual Machine Execution Loop]                                          |
|  Operand Stack:                                                                         |
|     [ 10 ]  ->  [ 10, 20 ]  ->  [ 30 ]  ->  [ 30, 3 ]  ->  [ 90 ]                       |
|                                                                                         |
+-----------------------------------------------------------------------------------------+
```

---

## 2. Deep Technical Dive & Memory Architecture

### The Grammar Specification (EBNF)
A recursive-descent parser uses a set of mutually recursive functions where each function directly implements a grammar production rule. To naturally enforce operator precedence without ambiguities, arithmetic expressions are partitioned into strict precedence strata:

```
Expression ::= Term ( ( '+' | '-' ) Term )*
Term       ::= Factor ( ( '*' | '/' ) Factor )*
Factor     ::= ( '+' | '-' ) Factor | Primary
Primary    ::= NUMBER | '(' Expression ')'
```

By placing `Term` inside `Expression`, multiplicative operators (`*`, `/`) are evaluated deeper in the call stack than additive operators (`+`, `-`), enforcing standard mathematical precedence ($* > +$) without ad-hoc rules.

### AST Memory Layout
Each node in the Abstract Syntax Tree is a dynamically allocated tagged union containing pointers to its sub-trees:

```text
               +----------------------------------+
               | ASTNode: BinaryOp ('*')          |
               | type = AST_BINARY_OP             |
               | op   = '*'                       |
               | left  ------------+  right  -----+--------+
               +-------------------|--------------+        |
                                   |                       |
                                   v                       v
               +-------------------------+    +-------------------------+
               | ASTNode: BinaryOp ('+') |    | ASTNode: Number (3.0)   |
               | type = AST_BINARY_OP    |    | type = AST_NUMBER       |
               | op   = '+'              |    | val  = 3.0              |
               | left ----+  right ----+ |    +-------------------------+
               +----------|------------|-+
                          |            |
                          v            v
               +----------------+  +----------------+
               | Number (10.0)  |  | Number (20.0)  |
               +----------------+  +----------------+
```

### Virtual Machine Operand Stack Architecture
The VM is an operand-stack machine. Unlike register machines (such as x86-64 or ARM) which specify operand registers in instructions (e.g., `add %eax, %ebx`), stack-based virtual machines implicitly operate on the top of the evaluation stack.

```text
Bytecode Stream:   OP_PUSH (10)  OP_PUSH (20)     OP_ADD        OP_PUSH (3)      OP_MUL
Instruction Ptr:        ^             ^              ^               ^              ^
                       (1)           (2)            (3)             (4)            (5)

Operand Stack:
  sp = 0           sp = 1        sp = 2         sp = 1          sp = 2         sp = 1
+--------+       +--------+    +--------+     +--------+      +--------+     +--------+
|        |       |        |    |  20.0  |     |        |      |   3.0  |     |        |
|--------|  -->  |--------| -> |--------| --> |--------|  ->  |--------| ->  |--------|
|        |       |  10.0  |    |  10.0  |     |  30.0  |      |  30.0  |     |  90.0  |
+--------+       +--------+    +--------+     +--------+      +--------+     +--------+
 (Empty)         Pushed 10     Pushed 20     Popped 20,10     Pushed 3       Popped 3,30
                                             Pushed 30                       Pushed 90
```

---

## 3. Instruction Set Architecture (ISA) & Opcode Specification

| Opcode | Binary Value | Operand(s) | Stack Transition | Operational Behavior |
|:---|:---:|:---|:---:|:---|
| `OP_HALT` | `0x00` | None | `[ ... ] -> [ ... ]` | Halts bytecode execution and returns `VM_OK`. |
| `OP_PUSH` | `0x01` | `uint16_t const_idx` | `[ ... ] -> [ ..., val ]` | Reads 16-bit constant index; pushes `constants[const_idx]`. |
| `OP_ADD` | `0x02` | None | `[ ..., a, b ] -> [ ..., (a + b) ]` | Pops `b` and `a`; pushes algebraic sum. |
| `OP_SUB` | `0x03` | None | `[ ..., a, b ] -> [ ..., (a - b) ]` | Pops `b` and `a`; pushes difference `(a - b)`. |
| `OP_MUL` | `0x04` | None | `[ ..., a, b ] -> [ ..., (a * b) ]` | Pops `b` and `a`; pushes product `(a * b)`. |
| `OP_DIV` | `0x05` | None | `[ ..., a, b ] -> [ ..., (a / b) ]` | Pops `b` and `a`; checks `fabs(b) < 1e-14`, traps on zero. |
| `OP_NEG` | `0x06` | None | `[ ..., a ] -> [ ..., (-a) ]` | Pops `a`; pushes algebraic negation `(-a)`. |
| `OP_PRINT` | `0x07` | None | `[ ..., a ] -> [ ... ]` | Pops `a`; formats and prints value to `stdout`. |
| `OP_POP` | `0x08` | None | `[ ..., a ] -> [ ... ]` | Pops top of stack and stores in `vm->last_result`. |
| `OP_DUP` | `0x09` | None | `[ ..., a ] -> [ ..., a, a ]` | Duplicates top element on the operand stack. |

---

## 4. Architectural Comparison Table

| Architecture Dimension | Tree-Walking AST Interpreter | Stack-Based Bytecode VM (Our Engine) | Register-Based VM (Lua 5 / Dalvik) |
|:---|:---|:---|:---|
| **Instruction Representation** | Pointer-linked heap tree nodes | Linear 1D flat byte buffer | Linear 32-bit fixed-width words |
| **Execution Mechanism** | Deep C call stack recursion | Flat while-switch dispatch loop | Flat while-switch or direct threading |
| **Data Storage** | Stack frames in host OS runtime | Fixed-size contiguous array (`Value*`) | Virtual register file array |
| **Cache Locality** | Extremely poor (heap pointer chasing) | Optimal ($L1$ instruction/data cache) | Optimal ($L1$ instruction/data cache) |
| **Disassembly / Inspection** | Tree traversal printout | Simple linear hex/mnemonic dump | Disassembler with 3-operand registers |
| **Implementation Complexity** | Low ($~100$ lines) | Moderate ($~300$ lines) | High ($~800+$ lines) |
| **Execution Performance** | Slowest ($1\times$ baseline) | Fast ($5\times - 15\times$ faster) | Fastest ($10\times - 20\times$ faster) |

---

## 5. Under the Hood: Compiler & VM Mechanics

### Instruction Dispatch Loop & Branch Prediction
In `vm_run()`, instruction execution is driven by a compact dispatch loop:
```c
while (vm->ip < code_end) {
    uint8_t instruction = *vm->ip++;
    switch (instruction) {
        case OP_ADD: { ... break; }
        case OP_MUL: { ... break; }
        ...
    }
}
```
The compiler transforms this `switch` into a jump table in the `.rodata` section. Modern superscalar CPUs predict the indirect jump target based on hardware Branch Target Buffers (BTB). Because instructions are packed contiguously into an 8-bit stream, instruction fetching enjoys nearly 100% $L1$ instruction cache hits.

### Dynamic Memory Safety Guarantees
1. **Constant Pool Deduplication**: When emitting constants (`chunk_add_constant`), the engine checks existing literals using `fabs(constants[i] - val) < EPSILON`. This bounds constant memory to $O(U)$ where $U$ is unique constants.
2. **Deterministic Heap Cleanup**:
   - `ast_free(root)` performs post-order traversal to deallocate all sub-trees.
   - `chunk_free(&chunk)` deallocates the byte buffer and constant array.
   - `vm_free(vm)` deallocates the operand stack and VM structure.
3. **Division by Zero Trapping**: Before computing `a / b`, `fabs(b) < 1e-14` is tested. If zero is detected, `VM_ERR_DIVISION_BY_ZERO` is returned immediately without raising hardware `SIGFPE`.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Left-Recursion Infinite Loops**:
   If a grammar rule begins with itself (e.g., `Expr -> Expr '+' Term`), a recursive-descent parser will recurse infinitely until the OS kills the process with stack overflow. Solution: convert left-recursion to iteration (`Expr -> Term ('+' Term)*`).
2. **Stack Underflow Traps**:
   Executing a binary operator (like `OP_ADD`) when the operand stack holds fewer than 2 elements causes memory corruption if unchecked. The VM explicitly validates `vm->sp < 2` before popping.
3. **Endianness Mismatch in 16-Bit Immediate Operands**:
   `OP_PUSH` takes a 16-bit constant index. Writing multi-byte integers directly via pointer cast (`*(uint16_t*)code`) causes endianness bugs across architectures (x86 Little-Endian vs PowerPC Big-Endian). The engine explicitly packs and unpacks bytes using big-endian bit shifts:
   ```c
   uint16_t c_idx = (uint16_t)((*vm->ip << 8) | *(vm->ip + 1));
   ```
4. **AST Memory Leaks on Syntax Errors**:
   If syntax parsing fails midway through a nested expression, previously allocated child nodes must not be orphaned. Our parser guarantees full subtree destruction via `ast_free` upon any syntax rejection.

---

## 7. Complexity & Production Conventions

- **Lexical Scanning**: $\mathcal{O}(N)$ time, where $N$ is input character count; $\mathcal{O}(1)$ auxiliary space.
- **Recursive-Descent Parsing**: $\mathcal{O}(N)$ deterministic linear time (no backtracking required for LL(1) arithmetic grammar); $\mathcal{O}(D)$ stack space where $D$ is expression nesting depth.
- **Bytecode Compilation**: $\mathcal{O}(M)$ time, where $M$ is AST node count; $\mathcal{O}(C)$ space for emitted bytecode chunk and constants.
- **VM Execution**: $\mathcal{O}(K)$ time, where $K$ is instruction count ($K \le 2M$); $\mathcal{O}(S)$ space where $S$ is max operand stack depth ($S \le D$).
- **Compilation Standard**: ISO C99 / C11, warning-clean under `gcc -Wall -Wextra -pthread -O2`.
