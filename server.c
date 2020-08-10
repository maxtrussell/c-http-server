#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main() {
	int success;
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Create socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("cannot create socket");
		return 1;
	}

	// set socket info
	memset((char *)&address, 0, sizeof(address));  // init to all 0's
	address.sin_family = AF_INET;                  // set address familty to IP
	address.sin_addr.s_addr = htonl(INADDR_ANY);   // 0.0.0.0
	address.sin_port = htons(PORT);                // set port

	// bind socket to port
	success = bind(server_fd, (struct sockaddr*) &address, sizeof(address));
	if (success < 0) {
		perror("bind failed");
		return 1;
	}

	// listen, buffer of 3 connections
	success = listen(server_fd, 3);
	if (success < 0) {
		perror("listening failed");
		return 1;
	}

	// accept conenctions
	int new_socket = accept(
		server_fd,
		(struct sockaddr*) &address,
		(socklen_t*) &addrlen
	);
	if (new_socket < 0) {
		perror("accepting failed");
		return 1;
	}
	
	// read from socket
	char buffer[1024] = {0};
	int valread = read(new_socket, buffer, 1024);
	printf("%s\n", buffer);
	if (valread < 0) {
		printf("There are no bytes to read\n");
	}

	// write to socket
	char *hello = "Hello from the server";
	write(new_socket, hello, strlen(hello));
	
	// cleanup
	close(new_socket);
	
	return 0;
}
