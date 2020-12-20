
.PHONY: all build test

all: build test

build: main

test: main
	./main

main: *.hpp *.cpp
	g++ *.cpp -g0 -O3 -lQuantLib -o main

