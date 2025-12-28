# ==========================================
# Variables and Flags
# ==========================================
CC = gcc
# Added -D_DEFAULT_SOURCE for usleep/nanosleep compatibility
CFLAGS = -O0 -g -mno-red-zone -fno-omit-frame-pointer -fPIC -Iinclude -D_DEFAULT_SOURCE
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
EX_DIR  = examples
TEST_DIR = tests

# Linker flags for using our own library
LDFLAGS = -L$(LIB_DIR) -lthread_lt -no-pie

# ==========================================
# Files
# ==========================================
# Library Objects
OBJS_C = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
OBJS_S = $(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.S))
ALL_OBJS = $(OBJS_C) $(OBJS_S)

# Example Executables (looks for examples/ex_*.c)
EX_SRCS = $(wildcard $(EX_DIR)/*.c)
EX_BINS = $(patsubst $(EX_DIR)/%.c, %, $(EX_SRCS))

# Benchmark Executables (looks for tests/bench*.c and tests/benchmark.c)
BENCH_SRCS = $(wildcard $(TEST_DIR)/bench*.c) $(wildcard $(TEST_DIR)/benchmark.c)
BENCH_BINS = $(patsubst $(TEST_DIR)/%.c, %, $(BENCH_SRCS))

# ==========================================
# Main Targets
# ==========================================
TARGET_STATIC = $(LIB_DIR)/libthread_lt.a
TARGET_SHARED = $(LIB_DIR)/libthread_lt.so

.PHONY: all clean directories examples benchmarks test

all: directories $(TARGET_STATIC) $(TARGET_SHARED) examples benchmarks test

directories:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)

# ==========================================
# Library Builds
# ==========================================
$(TARGET_STATIC): $(ALL_OBJS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Static Lib created: $@"

$(TARGET_SHARED): $(ALL_OBJS)
	$(CC) -shared -o $@ $^
	@echo "Shared Lib created: $@"

# Compile C source
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Assembly source
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

# ==========================================
# Examples & Benchmarks
# ==========================================

# 1. Build all examples
examples: $(EX_BINS)

# Rule for examples (links against static lib)
# e.g., make ex_simple
%: $(EX_DIR)/%.c $(TARGET_STATIC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 2. Build all benchmarks
benchmarks: $(BENCH_BINS)

# Rule for benchmarks
# e.g., make benchmark_contention
%: $(TEST_DIR)/%.c $(TARGET_STATIC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 3. Specific Test: Scheduler
# Maps tests/thread.test.c -> ./scheduler executable
test: scheduler
scheduler: $(TEST_DIR)/thread.test.c $(TARGET_STATIC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# ==========================================
# Utilities
# ==========================================
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) 
	rm -f $(EX_BINS) $(BENCH_BINS) scheduler

# context_switch test
# gcc -O0 -g -fno-omit-frame-pointer \
#     tests/context_switch.test.c \
#     src/context/context.c \
#     src/context/context_asm.S \
#     -o context_test

# flags = -O0 -g -mno-red-zone -Wimplicit-function-declaration -fno-omit-frame-pointer -no-pie
# #example builds
# gcc -no-pie -g -o ex_preempt ./examples/ex_preempt.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o ex_threadpool ./examples/ex_threadpool.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o ex_matrix ./examples/ex_matrix.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o ex_pipeline ./examples/ex_pipeline.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o ex_basic ./examples/ex_basic.c ./src/*.c ./src/context.S -Iinclude\

# #benchmarks
# gcc -no-pie -g -o benchmark_contention ./tests/bench.contention.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o benchmark_churn ./tests/bench.churn.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o benchmark_memory ./tests/bench.mem.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o benchmark_create ./tests/bench.create.c ./src/*.c ./src/context.S -Iinclude\
# gcc -no-pie -g -o benchmark ./tests/benchmark.c ./src/*.c ./src/context.S -Iinclude\

# #test
# gcc -o scheduler ./tests/thread.test.c ./src/*.c ./src/context.S -Iinclude