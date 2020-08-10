
build:
	gcc -Wall -o bin/server server.c

run: build
	./bin/main

clean:
	rm bin/*
