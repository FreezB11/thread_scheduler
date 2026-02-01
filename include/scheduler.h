///@file:scheduler.h
#pragma once
#include "thread_lt.h"
#include "queue.h"
#include <stdlib.h>
#include <stdatomic.h>

// DECLARE them as external (they live somewhere else)
// i hve to make to this TLS(thread local storage)
// for M:N modeling
extern __thread thread_lt    *current_thread;
extern __thread thread_lt    *main_thread;
extern          queue        ready_queue;
extern          queue        sleep_queue;
extern          queue        zombie_queue;
extern          atomic_int   nxt_tid;

void schedule();
