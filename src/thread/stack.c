#define STACK_SIZE (64 * 1024)

void *alloc_stack(void){
    void *stack = malloc(STACK_SIZE);
    return (char *)stack + STACK_SIZE; // <- top of the stack
}