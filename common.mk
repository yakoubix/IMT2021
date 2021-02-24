
.PHONY: all build test

all: build test

build: main

test: main
	./main

main: *.hpp *.cpp
	g++ *.cpp -std=c++17 -g0 -O3 -lQuantLib -o main

