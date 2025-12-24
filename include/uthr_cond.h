#pragma once
#include "uthr_mutex.h"

typedef struct uthr_cond uthr_cond_t;

void uthr_cond_init(uthr_cond_t *);
void uthr_cond_wait(uthr_cond_t *, uthr_mutex_t *);
void uthr_cond_signal(uthr_cond_t *);
void uthr_cond_broadcast(uthr_cond_t *);