# context_switch test
# gcc -O0 -g -fno-omit-frame-pointer \
#     tests/context_switch.test.c \
#     src/context/context.c \
#     src/context/context_asm.S \
#     -o context_test

flags = -O0 -g -mno-red-zone -Wimplicit-function-declaration -fno-omit-frame-pointer -no-pie
#example builds
gcc -no-pie -g -o ex_preempt ./examples/ex_preempt.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o ex_threadpool ./examples/ex_threadpool.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o ex_matrix ./examples/ex_matrix.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o ex_pipeline ./examples/ex_pipeline.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o ex_basic ./examples/ex_basic.c ./src/*.c ./src/context.S -Iinclude\

#benchmarks
gcc -no-pie -g -o benchmark_contention ./tests/bench.contention.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o benchmark_churn ./tests/bench.churn.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o benchmark_memory ./tests/bench.mem.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o benchmark_create ./tests/bench.create.c ./src/*.c ./src/context.S -Iinclude\
gcc -no-pie -g -o benchmark ./tests/benchmark.c ./src/*.c ./src/context.S -Iinclude\

#test
gcc -o scheduler ./tests/thread.test.c ./src/*.c ./src/context.S -Iinclude