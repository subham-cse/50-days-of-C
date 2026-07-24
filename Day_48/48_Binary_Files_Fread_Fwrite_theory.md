# Day 48: Binary File I/O: `fread()`, `fwrite()`, `fseek()`, and `ftell()`

## 1. Overview & Real-World Analogy
Text files encode data as ASCII or UTF-8 character sequences separated by newlines (`\r\n` or `\n`). In contrast, **binary files** store exact bit-for-bit representations of memory buffers (integers, floats, structs) without conversion overhead or delimiter parsing.

### Real-World Analogy
- **Text File**: Transcribing a musical symphony into written prose describing each note and word ("Note C at 120 BPM"). It is human-readable, but bulky and slow to interpret.
- **Binary File**: Saving the raw uncompressed audio stream (`.wav` format) or raw byte image. The machine can dump the entire memory snapshot straight onto disk and load it back with zero translation latency.

```
Integer 1234567890 (0x499602D2):
Text File Representation (10 ASCII bytes):   ['1','2','3','4','5','6','7','8','9','0']
Binary File Representation (4 raw bytes):    [ 0xD2 | 0x02 | 0x96 | 0x49 ] (Little-Endian)
```

---

## 2. Deep Technical Dive & Memory Streaming

### Signatures (`<stdio.h>`)
```c
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
```

- **`fread` / `fwrite` Return Value**: Returns the number of **full elements** (each of `size` bytes) successfully read or written, **NOT** the raw byte count.
- If return value $< \text{count}$, use `feof(stream)` or `ferror(stream)` to diagnose whether EOF or a disk error occurred.

### `fseek()` Origin Modes (`whence`)
1. `SEEK_SET`: Seek relative to the **beginning** of the file (offset $\ge 0$).
2. `SEEK_CUR`: Seek relative to the **current stream position**.
3. `SEEK_END`: Seek relative to the **end** of the file (e.g. `fseek(fp, 0, SEEK_END)` moves to EOF).

```
File Position Cursor Navigation:
+-----------------------------------------------------------------------------+
| Byte 0 | Byte 1 | Byte 2 | ... | Byte N-2 | Byte N-1 | EOF                  |
+-----------------------------------------------------------------------------+
^                                                      ^
SEEK_SET (Offset 0)                                    SEEK_END (Offset 0)
```

---

## 3. Calculating File Size with `fseek` and `ftell`

```c
fseek(fp, 0L, SEEK_END);
long file_size = ftell(fp);
fseek(fp, 0L, SEEK_SET); // Rewind back to beginning
```

---

## 4. Side-by-Side Comparison: Text Mode vs. Binary Mode

| Dimension | Text Files (`"r"`, `"w"`) | Binary Files (`"rb"`, `"wb"`) |
| :--- | :--- | :--- |
| **Data Format** | Human-readable ASCII / UTF-8 | Raw byte stream / Struct bit patterns |
| **Newline Translation** | Windows: `\n` $\leftrightarrow$ `\r\n` translation | Zero translation (Strict byte fidelity) |
| **Storage Efficiency**| Large for numbers (e.g. 10 chars for int) | Compact (Exact `sizeof(T)` bytes) |
| **Parsing Overhead** | High (`sscanf`, string tokenizer) | Near zero (Direct `memcpy` to/from disk) |
| **Random Access** | Impractical with variable length lines | Instant $O(1)$ random seek by struct offset |
| **Portability** | High across architectures | Low (Endianness & struct padding differences)|

---

## 5. Under the Hood: Endianness & Structure Padding Hazards

When saving raw C structs to disk via `fwrite(&my_struct, sizeof(my_struct), 1, fp)`:

1. **Structure Alignment & Padding**:
   Compilers insert invisible padding bytes to align structure members to CPU word boundaries (e.g., 4 or 8 bytes). Different compilers or CPU architectures may insert different padding bytes.
2. **Byte Endianness (Byte Order)**:
   - **Little-Endian (x86-64, ARM default)**: Least significant byte stored at lowest address.
   - **Big-Endian (Network Order, SPARC, PowerPC)**: Most significant byte stored first.
   - Writing binary data on Little-Endian and reading on Big-Endian reverses multi-byte integers unless explicit serialization (e.g., `htons()` / `htonl()`) is used.

```
32-bit Integer 0x12345678 in Memory:
Address:        0x00    0x01    0x02    0x03
Little-Endian:  [0x78]  [0x56]  [0x34]  [0x12]
Big-Endian:     [0x12]  [0x34]  [0x56]  [0x78]
```

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Forgetting Binary Flag `"b"` on Windows**:
   Opening a binary file in `"w"` or `"r"` mode on Windows causes `0x0A` (`\n`) to be converted to `0x0D 0x0A` (`\r\n`), corrupting binary payloads.
2. **Saving Pointers in Binary Files**:
   Writing a struct containing pointers (`char *str` or `Node *next`) only writes the memory address from the current process's address space. Reloading it in another run creates dangling pointers pointing to invalid virtual pages.
3. **Mismatched `fread()` Return Value**:
   Checking `if (fread(...) == sizeof(int))` instead of `if (fread(...) == 1)` (since `fread` returns element count, not byte count).

---

## 7. Complexity & Production Best Practices

- **Time Complexity**:
  - `fread()` / `fwrite()`: $O(K)$ where $K$ is total bytes transferred.
  - `fseek()`: $O(1)$ stream seek operation.
- **Production Tip**: Always define fixed-size integer types (`uint32_t`, `int64_t` from `<stdint.h>`) and explicit magic headers (e.g., `0x424D` for BMP) when architecting binary file formats.
