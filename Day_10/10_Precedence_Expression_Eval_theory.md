# Day 10: Operator Precedence, Associativity, and Order of Evaluation

## 1. Executive Overview & Mental Model
Writing predictable, standards-compliant C software requires a rigorous distinction between three frequently conflated concepts:
1. **Operator Precedence:** Dictates which operators bind more tightly to operands, determining the structural hierarchy of the Abstract Syntax Tree (AST).
2. **Associativity:** Dictates the grouping direction (Left-to-Right or Right-to-Left) when operators of *identical precedence* compete for operands.
3. **Order of Evaluation:** Dictates the runtime chronological sequence in which the CPU executes subexpressions or evaluates function arguments.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Mathematical Order of Operations vs Assembly Lines  |
| In \(2 + 3 \times 4\), precedence groups \(3 \times 4\) first.         |
| However, if the expression is \(f(a) + g(b) \times h(c)\), the AST     |
| multiplies the results of \(g\) and \(h\) before adding \(f\), but the |
| operating system/compiler may execute \(f(a)\), \(g(b)\), or \(h(c)\)  |
| in ANY chronological sequence!                                         |
+------------------------------------------------------------------------+
```

---

## 2. Complete 15-Level Precedence and Associativity Table

```
Level | Operators                            | Description                           | Associativity
------+--------------------------------------+---------------------------------------+--------------
1     | `()` `[]` `->` `.` `++` `--` (postfix)| Function call, array, member, postfix | Left-to-Right
2     | `++` `--` (prefix) `+` `-` (unary)   | Prefix inc/dec, unary signs, NOT,     | Right-to-Left
      | `!` `~` `*` (deref) `&` (addr)       | bitwise NOT, pointer dereference,     |
      | `(type)` `sizeof` `_Alignof`         | cast, sizing, alignment               |
3     | `*` `/` `%`                          | Multiplicative                        | Left-to-Right
4     | `+` `-` (binary)                     | Additive                              | Left-to-Right
5     | `<<` `>>`                            | Bitwise Shift Left / Right            | Left-to-Right
6     | `<` `<=` `>` `>=`                    | Relational inequality                 | Left-to-Right
7     | `==` `!=`                            | Relational equality                   | Left-to-Right
8     | `&`                                  | Bitwise AND                           | Left-to-Right
9     | `^`                                  | Bitwise XOR                           | Left-to-Right
10    | `|`                                  | Bitwise OR                            | Left-to-Right
11    | `&&`                                 | Logical AND (Short-Circuit)           | Left-to-Right
12    | `||`                                 | Logical OR (Short-Circuit)            | Left-to-Right
13    | `?:`                                 | Ternary Conditional                   | Right-to-Left
14    | `=` `+=` `-=` `*=` `/=` `%=`         | Compound Assignment                   | Right-to-Left
      | `<<=` `>>=` `&=` `^=` `|=`           |                                       |
15    | `,`                                  | Comma Operator                        | Left-to-Right
```

---

## 3. Precedence vs. Order of Evaluation: The Critical Distinction

```
Expression:  result = f() + g() * h();

1. AST Grammar Construction (Governed by Precedence):
              [ = ]
             /     \
      [ result ]   [ + ]
                  /     \
               [f()]    [ * ]
                       /     \
                    [g()]   [h()]

2. Runtime Execution Order (UNSPECIFIED by ISO C Standard!):
   Compiler A may execute:  f() -> g() -> h() -> multiply -> add
   Compiler B may execute:  h() -> g() -> f() -> multiply -> add
   Compiler C may execute:  g() -> h() -> multiply -> f() -> add
```

> [!CAUTION]
> If `f()`, `g()`, and `h()` mutate shared global state, the resulting program output is **non-deterministic** and undefined across compiler versions.

---

## 4. Sequence Points (C99) & Sequenced-Before Relations (C11)

A **Sequence Point** is a boundary in execution where all side effects of previously evaluated expressions are guaranteed to be fully complete, and no side effects of subsequent expressions have yet occurred.

### Locations of Sequence Points:
1. At the end of a full expression (statement-ending semicolon `;`).
2. At the `&&`, `||`, and comma `,` operators.
3. At the `?:` ternary conditional operator (after condition evaluation).
4. Immediately before a function call executes (after all arguments are evaluated).

### The Unsequenced Modification Rule:
Between two consecutive sequence points, an object's value shall be modified at most once by expression evaluation, or read only to determine the value to be stored. Violating this rule yields **Undefined Behavior (UB)**.

```c
// CLASSIC UNDEFINED BEHAVIORS:
i = i++;           // UB: Modified twice without intervening sequence point
a[i] = i++;        // UB: 'i' is modified and read unsequenced
func(i++, i++);    // UB: Order of argument evaluation is unsequenced
```

---

## 5. Five Notorious Precedence Traps

```
+-------------------------------------------------------------------------------------+
| Trap Code                  | Parsed Interpretation    | Intended Correct Pattern    |
+----------------------------+--------------------------+-----------------------------+
| `*ptr++`                   | `*(ptr++)` (advances ptr)| `(*ptr)++` (increments data)|
| `*struct_ptr.field`        | `*(struct_ptr.field)`    | `(*struct_ptr).field` or `->`|
| `val & MASK == 0`          | `val & (MASK == 0)`      | `(val & MASK) == 0`         |
| `c = getchar() != EOF`     | `c = (getchar() != EOF)` | `(c = getchar()) != EOF`    |
| `a << 2 + 1`               | `a << (2 + 1)` (Shift 3) | `(a << 2) + 1`              |
+----------------------------+--------------------------+-----------------------------+
```

---

## 6. Under the Hood: AST Construction and Register Allocation
During compiler parsing:
1. Expressions are converted to Reverse Polish Notation (RPN) or an AST.
2. The code generator applies tree-rewriting rules (e.g. Sethi-Ullman algorithm) to evaluate subtrees requiring the highest register count first, minimizing expensive stack register spills (memory pressure).

---

## 7. Complexity & Production Best Practices
1. **Explicit Parenthesization:** Never rely on human memory for non-arithmetic precedence (e.g., bitwise + relational). Write `(val & MASK) == 0`.
2. **One Side-Effect Per Statement:** Never combine `++` or `--` inside complex arithmetic expressions or function arguments.
3. **Audit Pointer Dereferences:** Use `->` for member access via pointers to avoid `*p.field` bugs.
