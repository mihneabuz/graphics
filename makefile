CC = gcc
INCLUDE = -Iinclude
LIBS = -lglfw -lGL -lm
FLAGS = -Wall -Wextra -std=c11 $(INCLUDE)

MODE ?= debug
ifeq ($(MODE),debug)
	FLAGS += -g -O0
else ifeq ($(MODE),release)
	FLAGS += -O3
else
	$(error Unknown MODE "$(MODE)". Use 'debug' or 'release')
endif

INCLUDE_DIR = include

TEST_DIR = test
TEST_BIN = $(TEST_DIR)/tests

SRC_DIR = src
BIN_DIR = bin

GLAD_OBJ = $(BIN_DIR)/glad.o

TRIANGLE_BIN = $(BIN_DIR)/triangle

BINS = $(TRIANGLE_BIN)

# ================ COMMANDS ================

build: $(BINS) $(TEST_BIN)

check:
	clang-tidy $(INCLUDE_DIR)/*.h $(SRC_DIR)/*.c $(TEST_DIR)/tests.c

clean:
	rm -f $(TEST_BIN) $(GLAD_OBJ) $(BINS)

# ================ BINARIES ================

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(GLAD_OBJ): $(SRC_DIR)/glad.c | $(BIN_DIR)
	$(CC) $(FLAGS) -c $(SRC_DIR)/glad.c -o $@

$(TRIANGLE_BIN): $(SRC_DIR)/triangle.c $(GLAD_OBJ) | $(BIN_DIR)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_triangle: $(TRIANGLE_BIN)
	./$(TRIANGLE_BIN)

debug_triangle: $(TRIANGLE_BIN)
	gdb ./$(TRIANGLE_BIN)

-include $(BIN_DIR)/*.d

# ================ TESTS ================

test: $(TEST_BIN)
	./$(TEST_DIR)/tests

test_valgrind: $(TEST_BIN)
	valgrind --leak-check=full ./$(TEST_BIN)

test_gdb: $(TEST_BIN)
	gdb ./$(TEST_BIN)

$(TEST_BIN): $(TEST_DIR)/tests.c
	$(CC) $(FLAGS) $^ -o $@


.PHONY: clean check test test_valgrind test_gdb
