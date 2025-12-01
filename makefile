CC = gcc
INCLUDE = -Iinclude
LIBS = -lm -lz -lGL -lglfw -lassimp
FLAGS = -Wall -Wextra -std=c23 $(INCLUDE)

MODE ?= debug
ifeq ($(MODE),debug)
	FLAGS += -g -O0
else ifeq ($(MODE),release)
	FLAGS += -O3
else
	$(error Unknown MODE "$(MODE)". Use 'debug' or 'release')
endif

INCLUDE_DIR = include
INCLUDE_LOADER = $(INCLUDE_DIR)/gl_loader.h

TEST_DIR = test
TEST_BIN = $(TEST_DIR)/tests

SRC_DIR = src
BIN_DIR = bin

TRIANGLE_BIN = $(BIN_DIR)/triangle
TEXTURE_BIN = $(BIN_DIR)/texture
TRANSFORM_BIN = $(BIN_DIR)/transform
CAMERA_BIN = $(BIN_DIR)/camera
LIGHT1_BIN = $(BIN_DIR)/light1
LIGHT2_BIN = $(BIN_DIR)/light2
MODEL_BIN = $(BIN_DIR)/model

BINS = $(TRIANGLE_BIN) $(TEXTURE_BIN) $(TRANSFORM_BIN) $(CAMERA_BIN) $(LIGHT1_BIN) $(LIGHT2_BIN) $(MODEL_BIN)

# ================ COMMANDS ================

build: $(BINS) $(TEST_BIN)

check:
	clang-tidy $(INCLUDE_DIR)/*.h $(SRC_DIR)/*.c $(TEST_DIR)/tests.c

clean:
	rm -f $(INCLUDE_LOADER) $(TEST_BIN) $(BINS)

# ================ BINARIES ================

$(INCLUDE_LOADER):
	./loader/generate.py > $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TRIANGLE_BIN): $(SRC_DIR)/triangle.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_triangle: $(TRIANGLE_BIN)
	./$(TRIANGLE_BIN)

$(TEXTURE_BIN): $(SRC_DIR)/texture.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_texture: $(TEXTURE_BIN)
	./$(TEXTURE_BIN)

$(TRANSFORM_BIN): $(SRC_DIR)/transform.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_transform: $(TRANSFORM_BIN)
	./$(TRANSFORM_BIN)

$(CAMERA_BIN): $(SRC_DIR)/camera.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_camera: $(CAMERA_BIN)
	./$(CAMERA_BIN)

$(LIGHT1_BIN): $(SRC_DIR)/light1.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_light1: $(LIGHT1_BIN)
	./$(LIGHT1_BIN)

$(LIGHT2_BIN): $(SRC_DIR)/light2.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_light2: $(LIGHT2_BIN)
	./$(LIGHT2_BIN)

$(MODEL_BIN): $(SRC_DIR)/model.c | $(BIN_DIR) $(INCLUDE_LOADER)
	$(CC) $(FLAGS) $(LIBS) $^ -o $@

run_model: $(MODEL_BIN)
	./$(MODEL_BIN)

# ================ TESTS ================

test: $(TEST_BIN)
	./$(TEST_DIR)/tests

test_valgrind: $(TEST_BIN)
	valgrind --leak-check=full ./$(TEST_BIN)

test_gdb: $(TEST_BIN)
	gdb ./$(TEST_BIN)

$(TEST_BIN): $(TEST_DIR)/tests.c
	$(CC) $(FLAGS) $(LIBS) $^ -o $@


.PHONY: clean check test test_valgrind test_gdb
