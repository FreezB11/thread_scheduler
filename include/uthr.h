#pragma once
// #include <cstdint>

typedef struct uthr_thread uthr_thread_t;

int uthr_init(void);
// int uthr_create(uthr_thread_t **out, void(* fn)(void *), void *arg);
int uthr_create(void (*fn)(void*), void *arg);

void uthr_yield(void);
void uthr_sleep(uint64_t ms);
void uthr_join(uthr_thread_t *t);
void uthr_exit(void);