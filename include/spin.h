///@file: spin.h
#pragma once

// Lock stub for M:N preparation
typedef struct {
    int locked; // 0 = unlocked, 1 = locked
} Spinlock;

void spin_lock(Spinlock *lk) { (void)lk; /* atomic_exchange/loop in M:N */ }
void spin_unlock(Spinlock *lk) { (void)lk; /* atomic_store in M:N */ }