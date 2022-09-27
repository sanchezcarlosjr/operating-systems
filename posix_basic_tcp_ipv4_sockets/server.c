#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_OUTSTANDING_CONNECTION_REQUEST 5
#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("\nserver.out <Server Port>");
		return -1;
	}
	in_port_t serverPort = atoi(argv[1]);

	printf("\nStarting socket...\n\nListening on port %d", serverPort);
	struct sockaddr_in socketAddress = {
		.sin_family=AF_INET,
		.sin_addr={
			.s_addr=htonl(INADDR_ANY)
		},
		.sin_port=htons(serverPort)
	};
	int socketDescriptor = 0;

	if ((socketDescriptor = socket(socketAddress.sin_family, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("\n[ERROR] Socket creation failed.");
		return -1;
	}

	if (bind(socketDescriptor, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0) {
		printf("\n[ERROR] Socket bind failed.");
		return -1;
	}

	if (listen(socketDescriptor, MAX_OUTSTANDING_CONNECTION_REQUEST) < 0) {
		printf("\n[ERROR] Starting listening income connections failed.");
		return -1;
	}


	struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
	int clientDescriptor;
	if ((clientDescriptor = accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0) {
		printf("[ERROR] accept() failed");
		return -1;
	}



	printf("\nStarting communication because a client has connected to server.");
	char clientName[INET_ADDRSTRLEN];
	if (inet_ntop(socketAddress.sin_family, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) == NULL) {
		printf("[ERROR] Unable to client address");
		return -1;
	} 
	char buffer[BUFFER_SIZE];



	printf("\nReceiving message from client...");
	ssize_t numBytesRcvd = recv(clientDescriptor, buffer, BUFFER_SIZE, 0);
	if (numBytesRcvd < 0) {
		printf("\n[ERROR] receive() failed");
		return -1;
	}
	printf("\n");
	fputs(buffer, stdout);

	printf("\nClosing communication");

        close(clientDescriptor);
        close(socketDescriptor);

	return 0;
}
