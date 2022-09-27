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
		printf("\rserver.out <Server Port>\n");
		return 0;
	}
	in_port_t serverPort = atoi(argv[1]);

	printf("\rStarting socket...\n");
	struct sockaddr_in socketAddress = {
		.sin_family=AF_INET,
		.sin_addr={
			.s_addr=htonl(INADDR_ANY) // it puts server's IPv4 automatically
		},
		.sin_port=htons(serverPort)
	};

	int socketDescriptor = 0;
	if ((socketDescriptor = socket(socketAddress.sin_family, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("\r[ERROR] Socket creation failed.\n");
		return -1;
	}

	if (bind(socketDescriptor, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0) {
		printf("\r[ERROR] Socket bind failed.\n");
		return -1;
	}

	if (listen(socketDescriptor, MAX_OUTSTANDING_CONNECTION_REQUEST) < 0) {
		printf("\r[ERROR] Starting listening income connections failed.\n");
		return -1;
	}

	printf("\rListening on port %d\n", serverPort);

	struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
	int clientDescriptor;
	if ((clientDescriptor = accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0) {
		printf("\r[ERROR] accept() failed\n");
		return -1;
	}



	printf("\rStarting communication because a client has connected to server.\n");
	char clientName[INET_ADDRSTRLEN];
	if (inet_ntop(socketAddress.sin_family, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) == NULL) {
		printf("\r[ERROR] Unable to client address\n");
		return -1;
	} 

	printf("\rReceiving message from client...\n");
	char buffer[BUFFER_SIZE];
	if (recv(clientDescriptor, buffer, BUFFER_SIZE, 0) < 0) {
		printf("\r[ERROR] Reception failed.\n");
		return -1;
	}
	printf("\rMessage: %s\n", buffer);


	printf("\rClosing communication...\n");
        close(clientDescriptor);
        close(socketDescriptor);

	printf("\rServer closes comunnication.\n");

	return 0;
}
