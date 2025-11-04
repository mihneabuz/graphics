CC = gcc
INCLUDE = -I./include
FLAGS = -Wall -Wextra -std=c11 $(INCLUDE)

DEBUG_FLAGS = $(FLAGS) -g -O0
RELEASE_FLAGS = $(FLAGS) -O3

build: tests

check:
	clang-tidy include/* test/*

tests: test/tests.c
	$(CC) $(DEBUG_FLAGS) -o tests test/tests.c

test: tests
	./tests

valgrind: tests
	valgrind --leak-check=full ./tests

clean:
	rm tests

.PHONY: clean tests check
