.PHONY: test

BIN=bin
CC=g++
FLAGS=-Wall -fsanitize=address,undefined

all: main test

main: *.cpp
	$(CC) $(FLAGS) -o bin/main main.cpp

findkey: *.cpp
	$(CC) $(FLAGS) -o bin/findkey findkey.cpp

test: bin/test
	./bin/test

bin/test: bin test/*.h
	cxxtestgen --error-printer -o bin/test.cpp test/*.h
	$(CC) -o bin/test bin/test.cpp

bin:
	mkdir -p bin

clean:
	rm -r bin
