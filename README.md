# Thread Scheduler (libthread_lt)

A lightweight, educational userspace threading library for Linux. This project implements core operating system concepts—context switching, scheduling, and synchronization—completely in user space using C and Assembly.

It is designed for students, systems programmers, and anyone curious about how threading works "under the hood."

> **Note:** If you find this project educational or useful, please consider giving it a ⭐ Star!

---

## Features

- **Userspace Context Switching**: Implemented in x86_64 Assembly without relying on `ucontext.h`.
- **Preemptive Scheduling**: Round-robin scheduler with signal-based preemption.
- **Synchronization Primitives**:
  - Mutexes (`mutex_lock`, `mutex_unlock`)
  - Condition Variables (`cond_wait`, `cond_signal`)
- **Library Artifacts**: Builds both Static (`.a`) and Shared (`.so`) libraries for easy integration.
- **No External Dependencies**: Relies only on the standard C library.

## 🛠️ Building the Library

This project uses a standard `Makefile` to compile the library and examples.

Clone the repository
git clone https://github.com/FreezB11/thread_scheduler.git
cd thread_scheduler

Build the library and all examples
make

Build only the library (Static & Shared)
make lib/libthread_lt.a

text

## Usage

You can use **libthread_lt** in your own projects by including the header and linking the library.

### 1. Project Structure
Ensure your directory looks like this:
```
my_project/
├── main.c
├── include/ <-- Copy from this repo
└── lib/ <-- Copy libthread_lt.a from this repo
```
text

### 2. Example Code (`main.c`)
```c
#include "include/thread_lt.h"
#include <stdio.h>

void worker(void *arg) {
printf("Hello from thread %d!\n", (int)arg);
}

int main() {
thread_init(); // Initialize the scheduler

text
int id = 1;
thread_t *t1 = thread_create(worker, &id);

thread_join(t1);
printf("Thread finished execution.\n");

return 0;
}
```
### 3. Compilation

**Using Static Library (Recommended for simplicity):**
```bash
gcc -I./include main.c lib/libthread_lt.a -o my_app
./my_app
```

**Using Shared Library:**
```bash
gcc -I./include -L./lib main.c -lthread_lt -o my_app
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
./my_app
```

## Documentation

For a deep dive into the internal design, scheduler logic, and assembly implementation, please read the full documentation:
**[Read the Documentation (doc.md)](doc.md)**

## Running Tests & Benchmarks

The project includes several built-in examples and benchmarks to test scheduler performance.
```bash
# run the basic test
./ex_basic

# run a benchmark
./benchmark_contention
```

## 🤝 Contributing

Contributions are welcome! Whether it's fixing bugs, improving the scheduler algorithm, or adding documentation:
1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/amazing-feature`).
3. Commit your changes (`git commit -m 'Add some amazing feature'`).
4. Push to the branch (`git push origin feature/amazing-feature`).
5. Open a Pull Request.

---
*Created by [FreezB11](https://github.com/FreezB11)*