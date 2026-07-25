# Day 49: Command Line Arguments: `argc`, `argv`, and Option Parsing

## 1. Overview & Real-World Analogy
Command-line arguments allow users and external scripts to configure a program's behavior at launch without modifying source code or prompting interactive input. The host environment passes these parameters to `main()` via two canonical arguments: `argc` (*argument count*) and `argv` (*argument vector*).

### Real-World Analogy
Think of launching a spaceship:
- `argc`: The manifest stating exactly how many cargo crates are loaded.
- `argv`: An array of labeled conveyor belts (`char *[]`), where crate 0 (`argv[0]`) is the rocket itself (program executable path), crate 1 (`argv[1]`) is the destination (`--target=mars`), and crate 2 (`argv[2]`) is the payload weight (`--payload=5000`).

```
Terminal Command: ./my_app --verbose -n 50 output.txt

argc = 5
argv[0] ---> "./my_app\0"
argv[1] ---> "--verbose\0"
argv[2] ---> "-n\0"
argv[3] ---> "50\0"
argv[4] ---> "output.txt\0"
argv[5] ---> NULL (C Standard Guarantee)
```

---

## 2. Deep Technical Dive & Memory Architecture

### Standard `main` Signatures (ISO C99 §5.1.2.2.1)
```c
int main(void);
int main(int argc, char *argv[]);
// Or equivalently:
int main(int argc, char **argv);
```

### Memory Layout on the Stack
When the OS kernel (via `execve()` on POSIX or `CreateProcess()` on Windows) spawns a C process:
1. The kernel copies the environment strings and command-line string array onto the top of the initial process stack.
2. The C runtime startup code (`crt0.o`) parses the command line, builds the `char *argv[]` pointer table, terminates it with a `NULL` pointer (`argv[argc] == NULL`), and calculates `argc`.
3. Calls `main(argc, argv)`.

```
High Memory
+-------------------------------------------------------------+
| String Table:                                               |
| "./my_app\0" "--verbose\0" "-n\0" "50\0" "output.txt\0"     |
+-------------------------------------------------------------+
| argv Pointer Array:                                         |
| [ &"./my_app" | &"--verbose" | &"-n" | &"50" | &"output.txt" | NULL ]
|      ^                                                          ^
|   argv[0]                                                   argv[argc]
+-------------------------------------------------------------+
| argc = 5                                                    |
+-------------------------------------------------------------+
Low Memory (Callee Frame)
```

---

## 3. String Conversion & Numeric Parsing

All elements in `argv` are immutable or modifiable null-terminated C strings (`char *`). To convert arguments into numbers:

### `strtol` vs `atoi`

| Feature | `atoi(str)` | `strtol(str, &endptr, base)` |
| :--- | :--- | :--- |
| **Error Handling** | None (Returns 0 on invalid string or 0 value) | Full (Distinguishes invalid string from literal 0) |
| **Overflow Detection**| Undefined behavior on overflow | Sets `errno = ERANGE`, returns `LONG_MIN` / `LONG_MAX` |
| **Radix Support** | Base 10 only | Arbitrary base (2 to 36, or 0 for auto-detect hex/oct) |
| **Partial Match** | Silently truncates | `endptr` points to first unparsed character |
| **Recommendation** | **Do not use in production** | **Mandatory production standard** |

---

## 4. Option Parsing: Flags, Key-Value Pairs, and Positional Arguments

Command-line utilities typically encounter three argument classes:
1. **Boolean Flags (Short/Long)**: `-v`, `--verbose`, `-h`, `--help`.
2. **Key-Value Options**: `-o output.bin`, `--count=100`, `-t 4`.
3. **Positional Arguments**: Trailing file paths or targets (e.g., `input.txt`).

```
CLI Parser State Machine:
          [ Iterate argv[1] ... argv[argc-1] ]
                     |
         +-----------+-----------+
         | Starts with '-'?      |
        YES                      NO
         |                       |
    [ Parse Flag / Option ]   [ Positional Argument ]
```

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Out-of-Bounds `argv` Access (`argv[i]` where `i >= argc`)**:
   Accessing beyond `argc` without checking `argc` bounds or relying on `argv[argc] == NULL`.
2. **Assuming `argv[0]` is Always Populated**:
   In rare embedded environments or programmatic `execve(..., NULL, ...)`, `argc` can be 0 and `argv[0]` can be `NULL`.
3. **Using `atoi()` for User Input**:
   `atoi("0")` returns 0; `atoi("invalid_abc")` also returns 0! There is no mechanism to detect parse failures. Always use `strtol()`.

---

## 6. Complexity & Production Best Practices

- **Time Complexity**: $O(\sum \text{strlen}(argv[i]))$ to parse command line strings.
- **Space Complexity**: Zero heap allocation when using pointers into existing `argv` slices.
- **Production Standard**: Always provide a `--help` / `-h` usage printer function detailing synopsis, available flags, and default values.
