#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  if (argc == 3 || argc == 4) {
	  printf("\n./client.out <SERVER ADDRESS IPv4> <ECHO WORD> [<SERVER PORT>]");
	  return EXIT_FAILURE;
  }
  char *serverIPv4 = argv[1];
  char *message = argv[2];
  in_port_t serverPort = (argc == 4) ? atoi(argv[3]) : 7;

  fputs("\nCreating socket...", stdin);
  int socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socketDescriptor < 0) {
          fputs("\n[ERROR] socket failed", stderr);
	  return EXIT_FAILURE;
  }

  struct sockaddr_in serverAddress = {
	  .sin_family=AF_INET,
	  .sin_addr={
		  .s_addr=0
	  },
	  .sin_port=htons(serverPort)
  };
  if (inet_pton(serverAddress.sin_family, serverIPv4, &serverAddress.sin_addr.s_addr) <= 0) {
	  fputs("\n[ERROR] Unable server.", stderr);
	  return EXIT_FAILURE;
  }

  fputs("\nConnecting to server...", stdin);
  if (connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    fputs("\n[ERROR] The connection failed.", stderr);
    return EXIT_FAILURE;
  }

  fputs("\nSending message to server...", stdin);
  size_t messageLength = strlen(message);
  ssize_t numBytes = send(socketDescriptor, message, messageLength, 0);
  if (numBytes < 0) {
     fputs("\n[ERROR] send failed", stderr);
     return EXIT_FAILURE;
  }
  if (numBytes != messageLength) {
     fputs("\n[ERROR] send un expected number of bytes", stderr);
     return EXIT_FAILURE;
  }

  fputs("\nClosing connection...", stdin);
  close(socketDescriptor);
  return EXIT_SUCCESS;
}
