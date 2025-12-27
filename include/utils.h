///@file: utils.h
#define _GNU_SOURCE
#pragma once
#include <stdint.h>
#include <signal.h>

uint64_t current_time_ms();
void interrupts_disable();
void interrupts_enable();