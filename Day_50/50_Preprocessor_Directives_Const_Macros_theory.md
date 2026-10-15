# Day 50: POSIX Concurrency, Mutex Synchronization & Sandboxed Process Execution

## 1. Title & Architectural Overview
Writing resilient, production-grade systems in C requires mastering both **intra-process concurrency** (POSIX threads and synchronization primitives) and **inter-process resilience** (sandboxing, file descriptor redirection, and timeout supervision). High-reliability systems cannot afford to let an untrusted or runaway task crash the entire application or freeze indefinitely in an infinite loop.

### Real-World Analogy
Think of an operating control center:
1. **POSIX Threads (`pthread_t`)**: Specialized operators sharing the same control room (**shared virtual memory space**), communicating instantly by looking at the central whiteboard (**heap / globals**).
2. **Mutual Exclusion (`pthread_mutex_t`)**: A single physical key to the master controls; only one operator can hold the key to update telemetry at a time, preventing two operators from flipping conflicting switches simultaneously (**race conditions**).
3. **Process Sandboxing (`fork()` + `execvp()`)**: Dispatching an untrusted external experiment into an explosion-proof, isolated chamber (**isolated address space**). If the experiment blows up, the main control room remains undamaged.
4. **Timeout Supervisor (`waitpid(..., WNOHANG)`)**: A safety countdown clock. If an experiment does not complete within its allotted time budget, the supervisor triggers an emergency kill switch (`kill(pid, SIGKILL)`), preventing runaway reactions from locking up the facility.

```
+-----------------------------------------------------------------------------------------+
|                  POSIX THREAD POOL & PROCESS SUPERVISOR ARCHITECTURE                    |
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|       +-------------------------------------------------------------------+             |
|       |         Thread-Safe Task Queue  (Guarded by pthread_mutex_t)      |             |
|       |   [ Task 1 ] -> [ Task 2 ] -> [ Task 3 ] -> ... -> [ Task N ]     |             |
|       +-------------------------------------------------------------------+             |
|             |                       |                        |                          |
|             v (Atomic Pop)          v (Atomic Pop)           v (Atomic Pop)             |
|       +------------+          +------------+           +------------+                   |
|       |  Worker 1  |          |  Worker 2  |           |  Worker 3  |  (pthread_t)      |
|       +------------+          +------------+           +------------+                   |
|             |                       |                        |                          |
|             | [In-Process Compute]  | [fork() & execvp()]    | [Hung Process Probe]     |
|             v                       v                        v                          |
|       (CPU Workload)         Child Process            Child Process (Sleep 10)          |
|                               (Sandboxed)              (Isolated Address Space)         |
|                              freopen("/dev/null")                |                      |
|                                     |                            |                      |
|                              Clean Exit (0)             Supervisor Loop                 |
|                                     |                   waitpid(..., WNOHANG)           |
|                                     v                            v                      |
|                              waitpid() Reaped            Timeout (300ms) Exceeded!      |
|                                     |                            |                      |
|                                     |                   kill(pid, SIGKILL)              |
|                                     |                   waitpid(pid, &s, 0) (Reaped)    |
|                                     v                            v                      |
|       +-------------------------------------------------------------------+             |
|       |       Audit Metric Aggregator (gettimeofday() Microsecond Stats)  |             |
|       +-------------------------------------------------------------------+             |
|                                                                                         |
+-----------------------------------------------------------------------------------------+
```

---

## 2. Deep Technical Dive & Memory Architecture

### Thread vs Process Virtual Memory Models

```text
               PROCESS A (Shared Threads)                 PROCESS B (Sandboxed Child)
         +-------------------------------------+         +----------------------------+
0x7FFF.. | Thread 1 Stack  | Thread 2 Stack   |         | Child Process Stack        |
         +-------------------------------------+         +----------------------------+
         |            (Guard Page)             |         | (Copy-on-Write Pages)      |
         +-------------------------------------+         +----------------------------+
         |      Shared Dynamic Heap            |         | Private Heap               |
         +-------------------------------------+         +----------------------------+
         | .bss Segment (Uninitialized Global) |         | .bss Segment               |
         +-------------------------------------+         +----------------------------+
         | .data Segment (Initialized Global)  |         | .data Segment              |
         +-------------------------------------+         +----------------------------+
0x0040.. | .text Segment (Machine Code Instructions)     | .text Segment (from exec)  |
         +-------------------------------------+         +----------------------------+
```

1. **POSIX Threads**:
   - Share identical `.text`, `.data`, `.bss`, heap segments, and file descriptor tables.
   - Each thread possesses only a private CPU register context (Instruction Pointer, Stack Pointer) and its own thread stack frame.
   - Data race hazard: concurrent unsynchronized writes to heap/global memory cause undefined behavior.
2. **Process Sandboxing (`fork()`)**:
   - Duplicates the entire virtual memory space using hardware **Copy-On-Write (COW)** page tables.
   - Memory writes in the child process allocate separate physical frames, insulating the parent process.
   - Crash hazard insulation: if the child dereferences `NULL` or triggers a `SIGSEGV`, the parent supervisor survives unaffected.

---

## 3. Mutual Exclusion & Synchronization Mechanics

The `pthread_mutex_t` type implements mutual exclusion to protect critical sections:

```c
pthread_mutex_lock(&queue->mutex);
/* --- CRITICAL SECTION BEGIN --- */
/* Access or modify shared queue data safely */
Task task = queue->tasks[queue->head++];
/* --- CRITICAL SECTION END --- */
pthread_mutex_unlock(&queue->mutex);
```

### Under the Hood: Linux Futex (Fast Userspace Mutex)
- **Uncontended Case**: The mutex uses an atomic assembly instruction (`lock cmpxchg` on x86-64) to acquire the lock in user space in $\approx 10$ nanoseconds with **zero kernel transition**.
- **Contended Case**: If another thread already holds the lock, the thread invokes the `sys_futex()` system call, putting the calling thread to sleep on the OS kernel's wait queue until awakened by `pthread_mutex_unlock()`.

---

## 4. Process Sandboxing & Non-Blocking Supervision (`waitpid` WNOHANG)

When executing an untrusted job or external binary, the parent delegates execution to a supervised child process:

```c
pid_t pid = fork();
if (pid == 0) {
    /* CHILD: Suppress standard output and error streams */
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    freopen("/dev/null", "r", stdin);

    execvp(argv[0], argv);
    _exit(127); /* If exec fails */
}

/* PARENT: Non-blocking supervision loop */
int elapsed_ms = 0;
while (elapsed_ms < timeout_ms) {
    int status = 0;
    pid_t w = waitpid(pid, &status, WNOHANG);

    if (w == pid) {
        /* Child exited normally before timeout */
        return WEXITSTATUS(status);
    }
    usleep(10 * 1000); /* 10ms polling interval */
    elapsed_ms += 10;
}

/* TIMEOUT EXPIRED: Enforce SIGKILL */
kill(pid, SIGKILL);
waitpid(pid, &status, 0); /* Reap zombie process */
```

### Key Primitives Explained
- `freopen(DEV_NULL, "w", stdout)`: Closes file descriptor 1 (`stdout`) and associates it with the null device, cleanly suppressing child output without pipe allocation.
- `waitpid(pid, &status, WNOHANG)`: Immediately returns `0` if the child is still executing, allowing the parent to monitor execution duration without blocking its own thread.
- `kill(pid, SIGKILL)`: Sends an uncatchable termination signal to instantly kill runaway or deadlocked processes.
- `waitpid(pid, &status, 0)`: Harvests the dead child's exit status from the kernel process table, preventing **zombie process leaks**.

---

## 5. Architectural Comparison Table

| Feature Dimension | POSIX Threads (`pthreads`) | Forked Processes (`fork` / `exec`) | Asynchronous Event Loop (`epoll` / `kqueue`) |
|:---|:---|:---|:---|
| **Memory Isolation** | Shared address space (No isolation) | Separate address spaces (Complete isolation) | Shared address space |
| **Crash Blast Radius** | Single thread crash kills entire process | Child crash does not affect parent | Handler crash kills entire process |
| **IPC Overhead** | Zero (Direct memory read/write) | Moderate (Pipes, sockets, shared memory) | None (In-process callbacks) |
| **Context Switch Cost** | Lightweight ($\approx 1 - 2\ \mu\text{s}$) | Moderate ($\approx 5 - 10\ \mu\text{s}$, TLB flush) | Ultra-lightweight ($\approx 0.1\ \mu\text{s}$) |
| **Timeout Enforcement** | Complex (Thread cancellation hazards) | Simple & reliable (`kill(SIGKILL)`) | Cooperative (Relies on non-blocking I/O) |
| **Ideal Use Case** | High-throughput parallel CPU computing | Untrusted code execution & sandboxing | High-concurrency network I/O servers |

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Deadlock (AB-BA Inversion)**:
   If Thread 1 locks Mutex A then Mutex B, while Thread 2 locks Mutex B then Mutex A, both threads can freeze permanently. Solution: always acquire locks in a globally consistent hierarchical order.
2. **Zombie Process Accumulation**:
   Terminated child processes remain in the OS kernel's process table as zombies until their parent calls `wait()` or `waitpid()`. If thousands of sandboxed tasks are spawned without reaping, the OS exhausts its maximum PID limit (`/proc/sys/kernel/pid_max`).
3. **Data Races on Unprotected Shared State**:
   Accessing shared variables without holding a mutex violates ISO C11 memory model invariants and produces corrupted calculations due to compiler instruction reordering and CPU store buffering.
4. **I/O File Descriptor Leaks**:
   Children inherit all open file descriptors from the parent. Production sandboxes use `FD_CLOEXEC` on open files or explicitly close/redirect descriptors using `freopen` before calling `execvp`.

---

## 7. Complexity & Best Practices

- **Queue Enqueue / Dequeue**: $\mathcal{O}(1)$ time under mutex protection.
- **Worker Scalability**: Linear speedup up to the number of physical CPU cores (governed by Amdahl's Law: $S = \frac{1}{(1-p) + \frac{p}{N}}$).
- **Process Supervision Polling**: $\mathcal{O}(T / \Delta t)$ where $\Delta t = 10\text{ ms}$ step interval.
- **Resource Cleanup Guarantee**: Every `pthread_mutex_init` is matched by `pthread_mutex_destroy`; every `pthread_create` is paired with `pthread_join`; every forked PID is reaped with `waitpid`.
- **Standards Compliance**: POSIX.1-2008 / ISO C99, compiled warning-free with `gcc -Wall -Wextra -std=c99 -pthread -O2`.
