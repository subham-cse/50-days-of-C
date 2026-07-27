# 🚀 50 Days of C: Systems Programming & Core Computer Science Mastery

Welcome to the **50 Days of C** repository — an exhaustive, production-grade curriculum designed to take you from core programming primitives and memory models to advanced pointers, dynamic memory management, low-level file I/O, and preprocessor mastery.

Every single day is structured into a dedicated module containing:
1. **In-Depth Theory Guide (`*_theory.md`)**: Architectural concepts, memory layouts, CPU register/stack interactions, assembly-level insights, side-by-side comparison matrices, and common pitfalls.
2. **Production ANSI C99 Code (`*_code.c`)**: Standalone, memory-safe, thoroughly documented implementations containing multi-scenario tests, edge case handlers, and 0 memory leaks.

---

## 🛠️ Compilation & Development Guide

All source code files strictly follow the **ANSI C99 Standard** (`-std=c99`) with all compiler warnings enabled.

### Quick Compile & Run

To compile and run any day's implementation:

```bash
# General template
gcc -Wall -Wextra -std=c99 -pedantic Day_XX/<filename>_code.c -o solution
./solution

# For math library exercises (Days 18 & 19):
gcc -Wall -Wextra -std=c99 -pedantic Day_18/18_Math_Arithmetic_Powers_Sqrt_code.c -lm -o solution
./solution
```

### Sanitizer Testing (Clang / GCC)
For deep memory safety validation:
```bash
gcc -Wall -Wextra -std=c99 -fsanitize=address,undefined Day_XX/<filename>_code.c -o solution
./solution
```

---

## 📥 Getting Started: Full Environment Setup

Follow these steps to set up everything you need to compile and run the C code on your machine. Pick your operating system below.

### Step 1 — Install a C Compiler (GCC)

You need a **GCC** (GNU Compiler Collection) toolchain that supports the C99 standard.

<details>
<summary><strong>🪟 Windows</strong></summary>

**Option A — MSYS2 (Recommended)**

1. Download the MSYS2 installer from **[https://www.msys2.org](https://www.msys2.org)**.
2. Run the installer → accept defaults → finish.
3. Open the **MSYS2 UCRT64** terminal and run:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
4. Add GCC to your system `PATH`:
   - Open **Settings → System → About → Advanced system settings → Environment Variables**.
   - Under **System variables**, select `Path` → **Edit** → **New**.
   - Add: `C:\msys64\ucrt64\bin`
   - Click **OK** on all dialogs.
5. Open a **new** PowerShell or Command Prompt and verify:
   ```powershell
   gcc --version
   ```

**Option B — MinGW-w64 (Standalone)**

1. Download the latest MinGW-w64 build from **[https://winlibs.com](https://winlibs.com)** (pick the **UCRT — GCC 13+** release, Win64 zip).
2. Extract the zip to `C:\mingw64`.
3. Add `C:\mingw64\bin` to your system `PATH` (same steps as Option A, step 4).
4. Verify with `gcc --version` in a new terminal.

</details>

<details>
<summary><strong>🍎 macOS</strong></summary>

1. Open **Terminal** and install the Xcode Command Line Tools (includes Apple Clang, which is GCC-compatible):
   ```bash
   xcode-select --install
   ```
2. *(Optional)* For true GCC via Homebrew:
   ```bash
   # Install Homebrew if you don't have it:
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

   brew install gcc
   ```
   Homebrew installs GCC as `gcc-14` (version may vary). You can alias it:
   ```bash
   echo 'alias gcc="gcc-14"' >> ~/.zshrc && source ~/.zshrc
   ```
3. Verify:
   ```bash
   gcc --version
   ```

</details>

<details>
<summary><strong>🐧 Linux (Ubuntu / Debian / Fedora / Arch)</strong></summary>

```bash
# Ubuntu / Debian
sudo apt update && sudo apt install build-essential -y

# Fedora
sudo dnf groupinstall "Development Tools" -y

# Arch
sudo pacman -S base-devel

# Verify
gcc --version
```

</details>

---

### Step 2 — Install a Code Editor / IDE

| Editor / IDE | Download Link | Best For |
|:---|:---|:---|
| **Visual Studio Code** *(Recommended)* | [https://code.visualstudio.com](https://code.visualstudio.com) | Lightweight, extensible, excellent C support via extensions |
| **CLion** (JetBrains) | [https://www.jetbrains.com/clion](https://www.jetbrains.com/clion) | Full-featured C/C++ IDE with built-in debugger & CMake |
| **Code::Blocks** | [https://www.codeblocks.org/downloads](https://www.codeblocks.org/downloads) | Beginner-friendly, ships with MinGW on Windows |
| **Dev-C++** (Embarcadero) | [https://www.embarcadero.com/free-tools/dev-cpp](https://www.embarcadero.com/free-tools/dev-cpp) | Classic lightweight Windows IDE |
| **Vim / Neovim** | [https://neovim.io](https://neovim.io) | Terminal-based, for advanced users |

---

### Step 3 — Set Up Visual Studio Code for C Development

> If you chose a different IDE, skip to **Step 4**.

1. **Install VS Code** from [https://code.visualstudio.com](https://code.visualstudio.com).
2. **Install Required Extensions** — Open VS Code → click **Extensions** (`Ctrl+Shift+X`) and install:
   - 🔹 **C/C++** by Microsoft — IntelliSense, debugging, code browsing.
   - 🔹 **Code Runner** by Jun Han *(optional)* — one-click compile & run.
3. **Open this repository**:
   - **File → Open Folder…** → navigate to the `50-Days-of-C` folder → **Select Folder**.
4. **Configure the build task** *(one-time setup)*:
   - Press `Ctrl+Shift+P` → type **"Tasks: Configure Default Build Task"** → select **"Others"**.
   - Replace the contents of `.vscode/tasks.json` with:
     ```json
     {
       "version": "2.0.0",
       "tasks": [
         {
           "label": "Build C File",
           "type": "shell",
           "command": "gcc",
           "args": [
             "-Wall",
             "-Wextra",
             "-std=c99",
             "-pedantic",
             "-g",
             "${file}",
             "-o",
             "${fileDirname}/${fileBasenameNoExtension}",
             "-lm"
           ],
           "group": {
             "kind": "build",
             "isDefault": true
           },
           "problemMatcher": ["$gcc"],
           "detail": "Compile the currently open C file with full warnings"
         }
       ]
     }
     ```
5. **Configure the debugger** *(optional but highly recommended)*:
   - Create `.vscode/launch.json` with:
     ```json
     {
       "version": "0.2.0",
       "configurations": [
         {
           "name": "Debug C File",
           "type": "cppdbg",
           "request": "launch",
           "program": "${fileDirname}/${fileBasenameNoExtension}",
           "args": [],
           "stopAtEntry": false,
           "cwd": "${fileDirname}",
           "environment": [],
           "externalConsole": true,
           "MIMode": "gdb",
           "miDebuggerPath": "gdb",
           "setupCommands": [
             {
               "description": "Enable pretty-printing",
               "text": "-enable-pretty-printing",
               "ignoreFailures": true
             }
           ],
           "preLaunchTask": "Build C File"
         }
       ]
     }
     ```

   > **🪟 Windows users**: Change `"miDebuggerPath"` to `"C:/msys64/ucrt64/bin/gdb.exe"` (or wherever your MinGW GDB lives). Install GDB via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-gdb`.

   > **🍎 macOS users**: Change `"MIMode"` to `"lldb"` and remove the `"miDebuggerPath"` line entirely (LLDB ships with Xcode tools).

---

### Step 4 — Compile & Run a Day's Code

Pick the workflow that matches your setup:

#### ▶️ Method 1: VS Code Build Task (Recommended)

1. Open any `*_code.c` file (e.g., `Day_01/01_Intro_History_Structure_code.c`).
2. **Build**: Press `Ctrl+Shift+B` — compiles the file using the task from Step 3.
3. **Run**: Open the integrated terminal (`` Ctrl+` ``) and type:
   ```bash
   # Windows (PowerShell)
   .\Day_01\01_Intro_History_Structure_code.exe

   # macOS / Linux
   ./Day_01/01_Intro_History_Structure_code
   ```
4. **Debug**: Press `F5` to start debugging. Click the gutter next to any line number to set breakpoints.

#### ▶️ Method 2: Code Runner Extension (One-Click)

1. Open any `*_code.c` file.
2. Press `Ctrl+Alt+N` (or click the **▶ Play** button, top-right corner).
3. To enable interactive `stdin` input, add this to your VS Code `settings.json` (`Ctrl+Shift+P` → "Open User Settings JSON"):
   ```json
   {
     "code-runner.runInTerminal": true,
     "code-runner.executorMap": {
       "c": "gcc -Wall -Wextra -std=c99 -pedantic -g $fileName -o $fileNameWithoutExt -lm && ./$fileNameWithoutExt"
     }
   }
   ```

#### ▶️ Method 3: Terminal / Command Line (Any OS, Any Editor)

```bash
# Navigate to the repository root
cd path/to/50-Days-of-C

# Compile any day (general pattern)
gcc -Wall -Wextra -std=c99 -pedantic Day_05/05_DataTypes_Modifiers_Qualifiers_code.c -o day05
./day05

# For days that use <math.h> (Days 18, 19, 33, etc.) — add -lm:
gcc -Wall -Wextra -std=c99 -pedantic Day_18/18_Math_Arithmetic_Powers_Sqrt_code.c -o day18 -lm
./day18

# Windows PowerShell — use backslashes and .exe:
gcc -Wall -Wextra -std=c99 -pedantic Day_05\05_DataTypes_Modifiers_Qualifiers_code.c -o day05.exe
.\day05.exe
```

#### ▶️ Method 4: Code::Blocks

1. Open Code::Blocks → **File → Open** → select the `.c` file.
2. **Build → Build and Run** (or press `F9`).
3. If you get linker errors on math-heavy days, go to **Settings → Compiler → Linker settings → Add** → type `-lm` → **OK**.

#### ▶️ Method 5: CLion (JetBrains)

1. **File → Open** → select the `50-Days-of-C` folder.
2. Create a `CMakeLists.txt` at the project root:
   ```cmake
   cmake_minimum_required(VERSION 3.10)
   project(50_Days_of_C C)
   set(CMAKE_C_STANDARD 99)

   # Add any day as a target:
   add_executable(day01 Day_01/01_Intro_History_Structure_code.c)
   add_executable(day18 Day_18/18_Math_Arithmetic_Powers_Sqrt_code.c)
   target_link_libraries(day18 m)   # link math library
   ```
3. Select the target from the dropdown → click **Build** (🔨) → **Run** (▶).

---

### Step 5 — Verify Your Setup ✅

Run this quick smoke test to confirm everything works end-to-end:

```bash
gcc -Wall -Wextra -std=c99 -pedantic Day_01/01_Intro_History_Structure_code.c -o test_setup
./test_setup
```

| Result | Meaning |
|:---|:---|
| ✅ Formatted console output, no errors | You're all set! Start learning. |
| ❌ `gcc: command not found` | GCC is not in your `PATH` — revisit **Step 1**. |
| ❌ `No such file or directory` | Your terminal isn't in the `50-Days-of-C` folder — `cd` into it first. |
| ❌ `undefined reference to 'sqrt'` | You forgot `-lm` — add it at the end of the compile command. |

---

## 🗺️ Complete 50-Day Curriculum Roadmap

| Day | Topic / Concept | Theory Document | ANSI C99 Implementation | Core Concepts & Highlights |
|:---:|:---|:---|:---|:---|
| **01** | **Intro & Structure** | [Day_01 Theory](./Day_01/01_Intro_History_Structure_theory.md) | [Day_01 Code](./Day_01/01_Intro_History_Structure_code.c) | C History, Memory Segments, Virtual Address Layout, `main()` Signatures |
| **02** | **Execution Flow & Translators** | [Day_02 Theory](./Day_02/02_Execution_Flow_Translators_theory.md) | [Day_02 Code](./Day_02/02_Execution_Flow_Translators_code.c) | Preprocessor, Compiler, Assembler, Linker, ELF Binaries, Symbol Tables |
| **03** | **Keywords & Syntax** | [Day_03 Theory](./Day_03/03_Keywords_Syntax_theory.md) | [Day_03 Code](./Day_03/03_Keywords_Syntax_code.c) | Keyword Taxonomy (32->48+), Storage Duration, `restrict`, `volatile` |
| **04** | **Tokens & Literals** | [Day_04 Theory](./Day_04/04_Tokens_Identifiers_Constants_theory.md) | [Day_04 Code](./Day_04/04_Tokens_Identifiers_Constants_code.c) | Lexical Analysis, Maximal Munch, Literal Radices, Read-Only String Data |
| **05** | **Data Types & Modifiers** | [Day_05 Theory](./Day_05/05_DataTypes_Modifiers_Qualifiers_theory.md) | [Day_05 Code](./Day_05/05_DataTypes_Modifiers_Qualifiers_code.c) | ILP32/LP64/LLP64 Models, Two's Complement, IEEE 754 Floats, Type Promotion Traps |
| **06** | **I/O & Escape Sequences** | [Day_06 Theory](./Day_06/06_IO_EscapeSequences_theory.md) | [Day_06 Code](./Day_06/06_IO_EscapeSequences_code.c) | `printf`/`scanf`, Format Specifiers, Stream Buffering (`_IOFBF`), Security Hazards |
| **07** | **Arithmetic & Logical Operators** | [Day_07 Theory](./Day_07/07_Arithmetic_Relational_Logical_theory.md) | [Day_07 Code](./Day_07/07_Arithmetic_Relational_Logical_code.c) | Truncated Division, Modulo Sign Invariant, Boolean Short-Circuiting, ALU Flags |
| **08** | **Bitwise & Shift Operators** | [Day_08 Theory](./Day_08/08_Bitwise_Shift_Operators_theory.md) | [Day_08 Code](./Day_08/08_Bitwise_Shift_Operators_code.c) | Bitwise Masking, Logical vs Arithmetic Shifts, Kernighan Popcount, Power of 2 |
| **09** | **Ternary, Sizeof & Comma** | [Day_09 Theory](./Day_09/09_Ternary_Sizeof_Comma_theory.md) | [Day_09 Code](./Day_09/09_Ternary_Sizeof_Comma_code.c) | Branchless Ternary, `sizeof` Non-Evaluation, Array Decay, Comma Operator |
| **10** | **Precedence & Expression Eval** | [Day_10 Theory](./Day_10/10_Precedence_Expression_Eval_theory.md) | [Day_10 Code](./Day_10/10_Precedence_Expression_Eval_code.c) | 15 Precedence Levels, Associativity, Sequence Points, Precedence Pitfalls |
| **11** | **If / Else & Nested Branching** | [Day_11 Theory](./Day_11/11_If_Else_Nested_theory.md) | [Day_11 Code](./Day_11/11_If_Else_Nested_code.c) | Branch Prediction, Dangling Else Problem, Nested Control Flow |
| **12** | **Else-If Ladders** | [Day_12 Theory](./Day_12/12_Else_If_Ladder_theory.md) | [Day_12 Code](./Day_12/12_Else_If_Ladder_code.c) | Multi-Way Selection, Mutually Exclusive Guards, Compiler Optimization |
| **13** | **Switch-Case Control** | [Day_13 Theory](./Day_13/13_Switch_Case_Control_theory.md) | [Day_13 Code](./Day_13/13_Switch_Case_Control_code.c) | Jump Tables, Binary Decision Trees, Fallthrough Mechanics, Duff's Device |
| **14** | **While Loop** | [Day_14 Theory](./Day_14/14_While_Loop_theory.md) | [Day_14 Code](./Day_14/14_While_Loop_code.c) | Entry-Controlled Iteration, Loop Invariants, Guarded Loops, Termination |
| **15** | **For Loop** | [Day_15 Theory](./Day_15/15_For_Loop_theory.md) | [Day_15 Code](./Day_15/15_For_Loop_code.c) | 3-Part Header, C99 Variable Scope, Loop Unrolling, Induction Variables |
| **16** | **Do-While Loop** | [Day_16 Theory](./Day_16/16_Do_While_Loop_theory.md) | [Day_16 Code](./Day_16/16_Do_While_Loop_code.c) | Exit-Controlled Iteration, Menu-Driven Workflows, `do-while(0)` Macro Idiom |
| **17** | **Break, Continue & Goto** | [Day_17 Theory](./Day_17/17_Break_Continue_Goto_theory.md) | [Day_17 Code](./Day_17/17_Break_Continue_Goto_code.c) | Unconditional Jumps, Loop Skipping, Error Cleanup Dispatch Pattern |
| **18** | **Math: Powers, Sqrt & Rounding** | [Day_18 Theory](./Day_18/18_Math_Arithmetic_Powers_Sqrt_theory.md) | [Day_18 Code](./Day_18/18_Math_Arithmetic_Powers_Sqrt_code.c) | `<math.h>`, `-lm`, Floating-Point Precision, `sqrt`, `pow`, `hypot`, `fabs` |
| **19** | **Math: Logs, Trig & Floating-Point** | [Day_19 Theory](./Day_19/19_Math_Logs_Trig_Floor_Ceil_theory.md) | [Day_19 Code](./Day_19/19_Math_Logs_Trig_Floor_Ceil_code.c) | `log`, `log10`, `exp`, `sin`, `cos`, `atan2`, `floor`, `ceil`, IEEE 754 Special Values |
| **20** | **1D Arrays Basics** | [Day_20 Theory](./Day_20/20_1D_Arrays_Basics_theory.md) | [Day_20 Code](./Day_20/20_1D_Arrays_Basics_code.c) | Contiguous Memory Layout, Array-to-Pointer Decay, Bounds Checking |
| **21** | **2D Arrays & Matrices** | [Day_21 Theory](./Day_21/21_2D_Arrays_Matrices_theory.md) | [Day_21 Code](./Day_21/21_2D_Arrays_Matrices_code.c) | Row-Major Layout, Flat Index Mapping `(i * COLS + j)`, Cache Locality |
| **22** | **Matrix Operations** | [Day_22 Theory](./Day_22/22_Matrix_Operations_theory.md) | [Day_22 Code](./Day_22/22_Matrix_Operations_code.c) | Matrix Addition, Transposition, $O(N^3)$ Multiplication, Loop Tiling |
| **23** | **Strings Basics & I/O** | [Day_23 Theory](./Day_23/23_Strings_Basics_IO_theory.md) | [Day_23 Code](./Day_23/23_Strings_Basics_IO_code.c) | Null-Terminator `\0`, String Literals vs Stack Buffers, Safe `fgets` |
| **24** | **String Utilities (`string.h`)** | [Day_24 Theory](./Day_24/24_Strlen_Strcpy_Strcat_Strcmp_theory.md) | [Day_24 Code](./Day_24/24_Strlen_Strcpy_Strcat_Strcmp_code.c) | `strlen`, `strcpy`, `strcat`, `strcmp`, Buffer Overflow Defenses |
| **25** | **String Reversals & Case Transforms** | [Day_25 Theory](./Day_25/25_Strrev_Case_Transformations_theory.md) | [Day_25 Code](./Day_25/25_Strrev_Case_Transformations_code.c) | Two-Pointer In-Place Reversal, ASCII Bitmasking Transforms, Word Reversal |
| **26** | **User-Defined Functions** | [Day_26 Theory](./Day_26/26_User_Defined_Functions_theory.md) | [Day_26 Code](./Day_26/26_User_Defined_Functions_code.c) | Function Anatomy, Calling Conventions (cdecl, System V AMD64 ABI) |
| **27** | **Call by Value vs Reference** | [Day_27 Theory](./Day_27/27_Call_By_Value_vs_Reference_theory.md) | [Day_27 Code](./Day_27/27_Call_By_Value_vs_Reference_code.c) | Value Copying vs Address Dereferencing, Side-Effect Mutation Safety |
| **28** | **Prototypes, Scope & Lifetime** | [Day_28 Theory](./Day_28/28_Function_Prototypes_Scope_theory.md) | [Day_28 Code](./Day_28/28_Function_Prototypes_Scope_code.c) | Forward Declarations, Header Guards, Block vs File Scope, Lifetime |
| **29** | **Recursion: Factorial** | [Day_29 Theory](./Day_29/29_Recursion_Factorial_theory.md) | [Day_29 Code](./Day_29/29_Recursion_Factorial_code.c) | Call Stack Frames, Base Conditions, Tail Call Optimization (TCO) |
| **30** | **Recursion: Natural Sum** | [Day_30 Theory](./Day_30/30_Recursion_Sum_Natural_Numbers_theory.md) | [Day_30 Code](./Day_30/30_Recursion_Sum_Natural_Numbers_code.c) | Linear Recurrence, Recurrence Trees, Stack Overflow Boundaries |
| **31** | **Recursion: GCD** | [Day_31 Theory](./Day_31/31_Recursion_GCD_theory.md) | [Day_31 Code](./Day_31/31_Recursion_GCD_code.c) | Euclidean Algorithm, Bézout Identity, Extended GCD, Modular Inverses |
| **32** | **Recursion: Digit Sum & Root** | [Day_32 Theory](./Day_32/32_Recursion_Digit_Sum_Root_theory.md) | [Day_32 Code](./Day_32/32_Recursion_Digit_Sum_Root_code.c) | Recursive Digit Sum, Digital Root, Modulo 9 Congruence, Tail Recursion |
| **33** | **Recursion: Binary Exponentiation** | [Day_33 Theory](./Day_33/33_Recursion_Power_Exponentiation_theory.md) | [Day_33 Code](./Day_33/33_Recursion_Power_Exponentiation_code.c) | $O(\log N)$ Divide-and-Conquer Power, Modular Exponentiation |
| **34** | **Storage Classes: Auto & Register** | [Day_34 Theory](./Day_34/34_Auto_Register_Classes_theory.md) | [Day_34 Code](./Day_34/34_Auto_Register_Classes_code.c) | Automatic Stack Storage, Register Hinting, SSA Form, Address-of Ban |
| **35** | **Storage Classes: Static & Extern** | [Day_35 Theory](./Day_35/35_Static_Extern_Classes_theory.md) | [Day_35 Code](./Day_35/35_Static_Extern_Classes_code.c) | Static Persistence, Internal/External Linkage, `.data`/`.bss` Segments |
| **36** | **Structures & Memory Padding** | [Day_36 Theory](./Day_36/36_Structures_Basics_Padding_theory.md) | [Day_36 Code](./Day_36/36_Structures_Basics_Padding_code.c) | Natural Alignment, Internal/Tail Padding, `offsetof`, Struct Optimization |
| **37** | **Unions vs Structures** | [Day_37 Theory](./Day_37/37_Unions_vs_Structures_theory.md) | [Day_37 Code](./Day_37/37_Unions_vs_Structures_code.c) | Overlapping Memory Offsets, Tagged Variants, Endianness Detection |
| **38** | **Enums & Typedef** | [Day_38 Theory](./Day_38/38_Enums_Typedef_theory.md) | [Day_38 Code](./Day_38/38_Enums_Typedef_code.c) | Enumeration Bitmasks, State Machines, `typedef` Function Pointers |
| **39** | **Pointers: Syntax & Dereferencing** | [Day_39 Theory](./Day_39/39_Pointers_Syntax_Dereferencing_theory.md) | [Day_39 Code](./Day_39/39_Pointers_Syntax_Dereferencing_code.c) | Address-Of `&`, Dereference `*`, Const Pointers, MMU Page Protection |
| **40** | **Pointer Arithmetic & Array Duality** | [Day_40 Theory](./Day_40/40_Pointer_Arithmetic_Arrays_theory.md) | [Day_40 Code](./Day_40/40_Pointer_Arithmetic_Arrays_code.c) | `sizeof(T)` Scaling, `ptrdiff_t`, `arr[i] == *(arr + i)`, SIB Addressing |
| **41** | **Pointer to Pointer (Double Pointers)** | [Day_41 Theory](./Day_41/41_Pointer_To_Pointer_theory.md) | [Day_41 Code](./Day_41/41_Pointer_To_Pointer_code.c) | Multi-Level Indirection, Indirect Modifications, Jagged 2D Arrays |
| **42** | **Void, NULL & Dangling Pointers** | [Day_42 Theory](./Day_42/42_Void_Null_Wild_Dangling_Pointers_theory.md) | [Day_42 Code](./Day_42/42_Void_Null_Wild_Dangling_Pointers_code.c) | Generic `void*`, `NULL` Sentinel, Use-After-Free Defenses, `SAFE_FREE` |
| **43** | **Dynamic Memory: Malloc & Free** | [Day_43 Theory](./Day_43/43_Malloc_Free_theory.md) | [Day_43 Code](./Day_43/43_Malloc_Free_code.c) | Heap Segment, Allocator Chunks (ptmalloc), `brk`/`sbrk` vs `mmap`, OOM Guards |
| **44** | **Dynamic Memory: Calloc Zero Init** | [Day_44 Theory](./Day_44/44_Calloc_Zero_Init_theory.md) | [Day_44 Code](./Day_44/44_Calloc_Zero_Init_code.c) | Zero-Initialization, Integer Multiplication Overflow Defense, OS Zero Page |
| **45** | **Dynamic Memory: Realloc & Safety** | [Day_45 Theory](./Day_45/45_Realloc_Memory_Leaks_theory.md) | [Day_45 Code](./Day_45/45_Realloc_Memory_Leaks_code.c) | Dynamic Vector Growth, In-Place vs Relocation, Temp Pointer Idiom |
| **46** | **File Streams: Open & Close** | [Day_46 Theory](./Day_46/46_File_Streams_Open_Close_theory.md) | [Day_46 Code](./Day_46/46_File_Streams_Open_Close_code.c) | `FILE*` Stream Handles, OS Descriptors, Buffer Modes, `errno` Diagnostics |
| **47** | **Formatted File I/O** | [Day_47 Theory](./Day_47/47_Formatted_IO_Fprintf_Fscanf_theory.md) | [Day_47 Code](./Day_47/47_Formatted_IO_Fprintf_Fscanf_code.c) | `fprintf`, `fscanf`, Scanset Regex Matching, Safe CSV Parser |
| **48** | **Binary File Streams & Random Access** | [Day_48 Theory](./Day_48/48_Binary_Files_Fread_Fwrite_theory.md) | [Day_48 Code](./Day_48/48_Binary_Files_Fread_Fwrite_code.c) | Raw Serialization (`fread`/`fwrite`), `fseek`, `ftell`, Random Access DB |
| **49** | **Command-Line Arguments** | [Day_49 Theory](./Day_49/49_Command_Line_Arguments_theory.md) | [Day_49 Code](./Day_49/49_Command_Line_Arguments_code.c) | `argc`/`argv` Stack Layout, CLI State Machine, Flag/Option Parsing |
| **50** | **Preprocessor & Macro Mastery** | [Day_50 Theory](./Day_50/50_Preprocessor_Directives_Const_Macros_theory.md) | [Day_50 Code](./Day_50/50_Preprocessor_Directives_Const_Macros_code.c) | `#define`, Stringizing `#`, Token-Pasting `##`, Header Guards, X-Macros |

---

## 🏛️ Repository Architecture

```text
50-Days-of-C/
├── README.md
├── Day_01/
│   ├── 01_Intro_History_Structure_theory.md
│   └── 01_Intro_History_Structure_code.c
├── Day_02/
│   ├── 02_Execution_Flow_Translators_theory.md
│   └── 02_Execution_Flow_Translators_code.c
...
└── Day_50/
    ├── 50_Preprocessor_Directives_Const_Macros_theory.md
    └── 50_Preprocessor_Directives_Const_Macros_code.c
```

---

## 🎯 Best Practices Followed
- **100% ANSI C99 Conformance**: Zero compiler warnings under `gcc -Wall -Wextra -std=c99 -pedantic`.
- **Memory Safety Guardrails**: Strict heap verification (`NULL` return checks, zero dangling references, matching `free()`, all `FILE*` streams closed).
- **Deep Educational Value**: Every single theory doc features memory diagrams, assembly/CPU architecture breakdowns, and complexity bounds.
