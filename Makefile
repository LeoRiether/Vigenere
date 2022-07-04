.PHONY: test

all: main test

main: bin/main

bin/main: main.cpp
	g++ -o bin/main main.cpp

test: bin/test
	./bin/test

bin/test: bin test/*.h
	cxxtestgen --error-printer -o bin/test.cpp test/*.h
	g++ -o bin/test bin/test.cpp

bin:
	mkdir -p bin

clean:
	rm -r bin
