///@file:scheduler.h
#pragma once
#include "thread_lt.h"
#include "queue.h"
#include <stdlib.h>

// DECLARE them as external (they live somewhere else)
extern thread_lt    *current_thread;
extern thread_lt    *main_thread;
extern queue        ready_queue;
extern queue        sleep_queue;
extern queue        zombie_queue;
extern int          nxt_tid;

void schedule();
