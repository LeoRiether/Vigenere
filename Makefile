.PHONY: test

BIN=bin
CC=g++

all: main test

main: main.cpp
	$(CC) -o bin/main -Wall -fsanitize=address,undefined main.cpp

test: bin/test
	./bin/test

bin/test: bin test/*.h
	cxxtestgen --error-printer -o bin/test.cpp test/*.h
	$(CC) -o bin/test bin/test.cpp

bin:
	mkdir -p bin

clean:
	rm -r bin
