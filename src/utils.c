///@file:utils.c
#include <utils.h>

#include <time.h>
#include <stdint.h>
#include <linux/time.h>

uint64_t current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void interrupts_disable() {
    sigset_t mask;
    sigfillset(&mask); // Block ALL signals
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void interrupts_enable() {
    sigset_t mask;
    sigemptyset(&mask); // Unblock ALL
    sigprocmask(SIG_SETMASK, &mask, NULL);
}
