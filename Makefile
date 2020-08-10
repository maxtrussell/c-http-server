
server:
	gcc -Wall -o bin/server server.c

run-server: server
	./bin/server

client:
	gcc -Wall -o bin/client client.c

run-client: client
	./bin/client

clean:
	rm bin/*
