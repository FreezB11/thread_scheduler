#pragma once

typedef struct uthr_mutex uthr_mutex_t;

void uthr_mutex_init(uthr_mutex_t *);
void uthr_mutex_lock(uthr_mutex_t *);
void uthr_mutex_unlock(uthr_mutex_t *);