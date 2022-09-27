#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
	  printf("\r./client.out <SERVER ADDRESS IPv4> <ECHO WORD> [<SERVER PORT>]\n");
	  return EXIT_FAILURE;
  }
  char *serverIPv4 = argv[1];
  char *message = argv[2];
  in_port_t serverPort = (argc == 4) ? atoi(argv[3]) : 7;

  printf("\rCreating socket...\n");
  int socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socketDescriptor < 0) {
          fputs("\r[ERROR] socket failed\n", stderr);
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
	  fputs("\r[ERROR] Unable server.\n", stderr);
	  return EXIT_FAILURE;
  }

  printf("\rConnecting to server...\n");
  if (connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    fputs("\r[ERROR] The connection failed.\n", stderr);
    return EXIT_FAILURE;
  }

  printf("\rSending message to server...\n");
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

  printf("\rClosing connection...\n");
  close(socketDescriptor);
  return EXIT_SUCCESS;
}
