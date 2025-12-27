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