CC = gcc
INCLUDE = -I./include -lglfw -lGL -lm
FLAGS = -Wall -Wextra -std=c11 $(INCLUDE)

DEBUG_FLAGS = $(FLAGS) -g -O0
RELEASE_FLAGS = $(FLAGS) -O3

build: app tests

check:
	clang-tidy include/*.h test/tests.c src/main.c

run: app
	./app

app: src/main.c glad.o shaders
	$(CC) $(RELEASE_FLAGS) src/main.c glad.o -o app

glad.o: src/glad.c
	$(CC) $(RELEASE_FLAGS) -c src/glad.c -o glad.o

shaders: src/vertex.glsl
	rm -f include/shaders.h
	xxd -n vertexShaderSource -i src/vertex.glsl >> include/shaders.h
	xxd -n fragmentShaderSource -i src/fragment.glsl >> include/shaders.h

tests: test/tests.c
	$(CC) $(DEBUG_FLAGS) -o tests test/tests.c

test: tests
	./tests

valgrind: tests
	valgrind --leak-check=full ./tests

clean:
	rm -f app glad.o tests vgcore.*

.PHONY: clean tests check shaders
