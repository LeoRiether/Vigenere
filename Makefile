.PHONY: test

BIN=bin
CC=g++
FLAGS=-std=c++17 -Wall -fsanitize=address,undefined -O2

EXT=
ifeq ($(OS),Windows_NT)
	EXT:=.exe
endif

all: main test

main: bin *.cpp
	$(CC) $(FLAGS) -o bin/main$(EXT) main.cpp

findkey: bin *.cpp
	$(CC) $(FLAGS) -o bin/findkey$(EXT) findkey.cpp

test: bin/test
	./bin/test

bin/test: bin test/*.h
	cxxtestgen --error-printer -o bin/test.cpp test/*.h
	$(CC) -o bin/test$(EXT) bin/test.cpp

bin:
	mkdir -p bin

clean:
	rm -r bin
