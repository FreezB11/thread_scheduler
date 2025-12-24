# context_switch test
# gcc -O0 -g -fno-omit-frame-pointer \
#     tests/context_switch.test.c \
#     src/context/context.c \
#     src/context/context_asm.S \
#     -o context_test