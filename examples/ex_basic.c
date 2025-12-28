///@file: ex_basic.c
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>

void greet(void *arg){
    char *name = (char *)arg;
    for(int i =0; i < 3; i++){
        printf("hello from %s! (count %d)\n",name, i);
        thread_yield();
    }
}

int main(){
    init();
    printf("=======basic thread example=======\n");

    thread_lt *t1 = thread_create(greet, "ALICE");
    thread_lt *t2 = thread_create(greet, "BOB");

    thread_join(t1);
    thread_join(t2);

    printf("=======done=======\n");
    return 0;
}