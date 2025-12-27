#define _GNU_SOURCE
#include <signal.h>
#include <thread_lt.h>
#include <sys/time.h>
#include <stdio.h>
#include <scheduler.h> // Access to schedule()
#include <signal.h>

// The Signal Handler
void timer_handler(int signum) {
    (void)signum;
    // printf("DEBUG: Preemption Tick!\n"); // Debug only, remove later
    
    // Force a yield
    // Note: We are in an interrupt context here. 
    // Calling schedule() directly works IF schedule() doesn't hold locks 
    // that the interrupted thread was holding.
    
    // Since we are N:1, we don't have kernel-level locks to deadlock on 
    // (except maybe malloc lock if we interrupted malloc).
    
    // CRITICAL: We must ensure we are not interrupting the scheduler itself!
    // A simple flag can protect this.
    
    thread_yield(); 
}

void preemption_init(int time_ms) {
    struct sigaction sa;
    struct itimerval timer;

    // 1. Install Handler
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart syscalls if interrupted
    sigaction(SIGALRM, &sa, NULL);

    // 2. Configure Timer (50ms interval)
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = time_ms * 1000;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = time_ms * 1000;

    // 3. Start Timer
    setitimer(ITIMER_REAL, &timer, NULL);
}

void preemption_disable() {
    struct itimerval timer = {0};
    setitimer(ITIMER_REAL, &timer, NULL);
}
