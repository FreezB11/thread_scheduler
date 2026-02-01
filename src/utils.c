///@file:utils.c
#include <utils.h>

#include <time.h>
#include <stdint.h>
#include <linux/time.h>


static __thread int interrupt_depth = 0;
static __thread sigset_t saved_mask;

uint64_t current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void interrupts_disable() {
    if(interrupt_depth == 0){
        sigset_t mask;
        sigfillset(&mask); // Block ALL signals
        sigprocmask(SIG_BLOCK, &mask, &saved_mask);
    }
    interrupt_depth++;   
}

void interrupts_enable() {
    if(interrupt_depth > 0){
        interrupt_depth--;
        if(interrupt_depth == 0){
            sigprocmask(SIG_SETMASK, &saved_mask, NULL);
        }
    }
}
