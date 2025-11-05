CC = gcc
INCLUDE = -I./include -lglfw -lGL
FLAGS = -Wall -Wextra -std=c11 $(INCLUDE)

DEBUG_FLAGS = $(FLAGS) -g -O0
RELEASE_FLAGS = $(FLAGS) -O3

build: app tests

check:
	clang-tidy include/* test/*

run: app
	./app

app: src/main.c glad.o
	$(CC) $(RELEASE_FLAGS) -o app glad.o src/main.c

glad.o: src/glad.c
	$(CC) $(RELEASE_FLAGS) -c -o glad.o src/glad.c

tests: test/tests.c
	$(CC) $(DEBUG_FLAGS) -o tests test/tests.c

test: tests
	./tests

valgrind: tests
	valgrind --leak-check=full ./tests

clean:
	rm -f app glad.o tests vgcore.*

.PHONY: clean tests check
