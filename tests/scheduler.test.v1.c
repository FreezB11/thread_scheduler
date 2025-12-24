#include <stdio.h>
#include "../include/uthr.h"
#include "../src/scheduler/scheduler.h"

void worker1(void *arg){
    for(int i = 0; i < 5; i++){
        printf("worker1: %d\n",i);
        uthr_yield();
    }
}

void worker2(void *arg){
    for(int i = 0; i < 7; i++){
        printf("worker2: %d\n",i);
        uthr_yield();
    }
}

void worker3(void *arg){
    for(int i = 0; i < 3; i++){
        printf("worker3: %d\n",i);
        uthr_yield();
    }
}

int main(){
    uthr_create(worker1, NULL);
    uthr_create(worker2, NULL);
    uthr_create(worker3, NULL);
    scheduler_run();
    return 0;
}

/*
gcc -O0 -g -mno-red-zone -fno-omit-frame-pointer 
tests/scheduler.test.v1.c 
src/context/context.c 
src/context/context_asm.S 
src/scheduler/scheduler.c 
-o scheduler                                            ─╯
*/