#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_OUTSTANDING_CONNECTION_REQUEST 1
#define BUFFER_SIZE 512


int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 1) {
	  printf("\rchat.out <PEER ADDRESS IPv4> <PEER SERVER PORT>\n");
	  return EXIT_FAILURE;
  }
  char* peerIPv4 = argc == 1 ? "192.168.0.1" : argv[1];
  in_port_t peerPort = argc == 1 ? 7 : atoi(argv[2]);

  printf("\rStarting socket...\n");
  struct sockaddr_in socketAddress = {
	  .sin_family=AF_INET,
	  .sin_addr={
		  .s_addr=htonl(INADDR_ANY)
	  },
	  .sin_port=0 // bind() will assign a random port	
  };

  int socketDescriptor = 0;
  if ((socketDescriptor = socket(socketAddress.sin_family, SOCK_STREAM, IPPROTO_TCP)) < 0) {
	  printf("\r[ERROR] Socket creation failed.\n");
	  return EXIT_FAILURE;
  }

  if (bind(socketDescriptor, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0) {
	  printf("\r[ERROR] Socket bind failed.\n");
	  return EXIT_FAILURE;
  }

  if (listen(socketDescriptor, MAX_OUTSTANDING_CONNECTION_REQUEST) < 0) {
	  printf("\r[ERROR] Starting listening income connections failed.\n");
	  return EXIT_FAILURE;
  }



  struct sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);
  int clientDescriptor;

  struct sockaddr_in peerAddress = {
	  .sin_family=AF_INET,
	  .sin_addr={
		  .s_addr=0
	  },
	  .sin_port=htons(peerPort)
  };
  if (inet_pton(peerAddress.sin_family, peerIPv4, &peerAddress.sin_addr.s_addr) <= 0) {
	  fputs("\r[ERROR] Unable server.\n", stderr);
	  return EXIT_FAILURE;
  }

  printf("\rConnecting to peer...\n");
  if (connect(socketDescriptor, (struct sockaddr *) &peerAddress, sizeof(peerAddress)) < 0) {
	  fputs("\r[ERROR] The connection failed.\n", stderr);
	  struct sockaddr_in realSocketAddress;
	  socklen_t len = sizeof(realSocketAddress);
	  if (getsockname(socketDescriptor, (struct sockaddr *)&realSocketAddress, &len) == -1) {
		  printf("\r[ERROR] Your chat cannot find an available port.");
		  return EXIT_FAILURE;
	  }
	  printf("\rListening on port %d\n", realSocketAddress.sin_port);
	  if ((clientDescriptor = accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0) {
		  printf("\r[ERROR] accept() failed\n");
		  return EXIT_FAILURE;
	  }
	  printf("\rStarting communication because a peer has connected to your chat.\n");
	  char clientName[INET_ADDRSTRLEN];
	  if (inet_ntop(socketAddress.sin_family, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) == NULL) {
		  printf("\r[ERROR] Unable to client address\n");
		  return EXIT_FAILURE;
	  }
  }


  if(fork()==0) {
	  // Child receives messages from peer.
	  char buffer[BUFFER_SIZE];
	  if (recv(clientDescriptor, buffer, BUFFER_SIZE, 0) < 0) {
		  printf("\r[ERROR] Reception failed.\n");
		  return EXIT_FAILURE;
	  }
	  printf("\rMessage: %s\n", buffer);
	  exit(EXIT_SUCCESS);
  } else {
	  printf("\rSending message to peer...\n");
	  char* message;
	  scanf("% [^\n]s", message);
	  size_t messageLength = strlen(message);
	  ssize_t numBytes = send(socketDescriptor, message, messageLength, 0);
	  if (numBytes < 0) {
		  fputs("\r[ERROR] send failed\n", stderr);
		  return EXIT_FAILURE;
	  }
	  if (numBytes != messageLength) {
		  fputs("\r[ERROR] send un expected number of bytes\n", stderr);
		  return EXIT_FAILURE;
	  }
  }



  printf("\rClosing communication...\n");
  close(clientDescriptor);
  close(socketDescriptor);

  printf("\rYou have closeed comunnication.\n");


  return EXIT_SUCCESS;
}
