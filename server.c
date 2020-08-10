#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 2048
#define PORT 8080

int read_file(char* dst, char* path);

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

	int new_socket;
	while (1) {
		// accept conenctions
		new_socket = accept(
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

		printf("Request received:\n=================\n%s\n", buffer);

		if (valread < 0) {
			printf("There are no bytes to read\n");
		}

		// get content
		char content[BUFFER_SIZE];
		success = read_file(content, "index.html");
		if (success != 0) {
			perror("could not read file");
			return 1;
		}

		// write to socket
		char resp[BUFFER_SIZE];
		sprintf(
			resp,
			"HTTP/1.1 %d OK\n"
			"Content-Type: text/html\n"
			"Content-Length %ld\n\n"
			"%s",
			200,
			strlen(content),
			content
		);
		write(new_socket, resp, strlen(resp));
	
		// cleanup
		close(new_socket);
	}
	return 0;
}

int read_file(char* dst, char* path) {
	// open file
	FILE* file = fopen(path, "r");
	if (!file)
		return -1;
		
	// get file length
	fseek(file, 0L, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	// copy file into buffer
	int success = fread(dst, file_size, 1, file);
	if (success != 1) {
		fclose(file);
		fprintf(stderr, "failed to read file '%s'", path);
		return -1;
	}

	// cleanup
	fclose(file);

	return 0;
}
