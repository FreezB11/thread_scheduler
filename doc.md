# thread_scheduler
What is a thread scheduler\
    A cpu can execute only one instruction stream per core at a time, but we want:\
 * many tasks
 * responsivenes
 * fairness
 * blocking without freezing the program

 A thread_ schedule is the system that decides which thread runs next for how long and why
 ## Threads vs Process
`Process`: has its own memory, expensive to switch\
`Thread`: shares memory, cheap to switch

User-space threads(green threads):
* faster than kernel threads
* fully controlled by our runtime

## Why user-space scheduling exists
kernel threads:
* syscall overhead
* kernel decides scheduling
* harder to customize

User-space scheduler:
* faster context switches
* custom policies
* perfect for :
    * async server
    * runtimes
    * embedded sys

Scheduler runs above the kernel, but feels like a real 0S.

## Fundamental abstraction: the Thread
a thread is not magic. it is:
* **A stack**( memory for function calls)
* **A cpu context**(registers)
* **A state**
* **A place in queue**

we create a `thread_control_block` to hold this\
**Thread states**(this lets scheduler make correct decisions)
| State   | Meaning                      |
| ------- | ---------------------------- |
| READY   | Runnable, waiting for CPU    |
| RUNNING | Currently executing          |
| BLOCKED | Waiting (sleep, mutex, join) |
| DONE    | Finished execution           |

## Context Switching (the heart of everything)
**What is context?**\
The cpu has:
* registers
* stack ptr
* instruction ptr

to pause one thread and run another:
* save current registers
* restore another thread's registers
* jump to its instruction ptr

that's a context switch.\
Two approaches we can use:
* `ucontext`
* raw assembly
    * saves only required registers
    * faster
    * full control

this is the illusion of parallelism.

## Run Queues( how scheduling actually works)
the scheduler doesnt run the thread\
it manages queues.\
**Run queue**
* list of ready threads
* round-robin scheduling

scheduler loop:
* pick next thread
* switch
* it stops -> repeat

## Types of scheduling
### Co-operative scheduling
threads explicitly called yeild().\
**Advantages**
* simple
* predictable

**Problem**
A buggy thread can hog the CPU forever.

### Preemptive Scheduling
We add:
* hardware timer
* periodic interrupts

**How preemption works**
* kernel fires `SIGALRM`
* signal handler interupts current thread
* scheduler forces a context switch

Now:
* time slicing exists
* fairness is guaranteed
* threads dont need to behave

## Blocking and Sleeping
**The key insight**
`Sleeping is blocking, not waiting `\
Busy waiting = waste of cpu\
blocking = give cpu to others

**Sleep queue**\
Threads that call thread_sleep():
* are marked BLOCKED
* placed in a time-ordered sleep queue

Scheduler:
* checks time
* wakes threads when needed

this is efficient and scalable.

## Thread join ( synchronization primitive)
what is `join`?\
one thread waits for another to finish.\

implementing it
* target thread stores a ptr to its jointer
* on exit:
    * target wakes joiner
    * joiner moves to ready

this avoids:
* polling
* busy waiting
* race conditions

## Mutexes (mutual exclusion)
**The problem mutex solves**\
multiple threads accessing shared memory -> data races.\
**what a mutex really is**
* an owner
* a wait queue

no spinning, no kernel help.\
just blocking and waking

this is scheduler-aware synchronization.

## condition variables
mutexes protect data, condition variables coordinate events.\
example: "Wait until buffer not empty".\
**why they exist**\
Mutexes alone cannot:
* wait for conditions efficiently
* wake multiple threads cleanly

Condition variables add:
* wait queues
* signaling semantics

## Scheduler Locking (critical sections)

Your scheduler manipulates shared data:
* run queues
* sleep queues
* wait queues

To stay correct:
* block timer interrupts
* avoid reentrancy

This is similar to:
* disabling interrupts in kernels

## Raw Assembly Context Switching
This is the professional leap.\
You manually:
* save callee-saved registers
* restore another thread’s state
* jump to saved instruction pointer

This removes:
* libc dependency
* overhead
* unpredictability

## M:N Scheduling
Problem

Single OS thread = single core.

Solution
* N kernel threads
* M user threads
* user scheduler decides mapping

Each CPU:
* has its own run queue
* steals work if idle

This scales to all cores.