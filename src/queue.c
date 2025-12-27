///@file:queue.c
#include <queue.h>
#include <stdlib.h>
#include <stdio.h>

void queue_push(queue *q, thread_lt *t){
    // printf("hey we pushed the thread\n");
    t->next = NULL;
    if(q->tail){
        q->tail->next = t;
        q->tail = t;
    }else{
        q->head = q->tail = t;
    }
}

thread_lt* queue_pop(queue *q){
    if(!q->head) return NULL;
    thread_lt *t = q->head;
    q->head = t->next;
    if(!q->head) q->tail = NULL;
    t->next = NULL;
    return t;
}

// remove specific thread(used in sleep queue handling)
void queue_remove(queue *q, thread_lt *t){
    if(!q->head) return;
    if(q->head == t){
        queue_pop(q);
        return;
    }
    thread_lt *curr = q->head;
    while(curr->next && curr->next != t) curr = curr->next;
    if(curr->next == t){
        curr->next = t->next;
        if(t == q->tail) q->tail = curr;
        t->next = NULL;
    }
}