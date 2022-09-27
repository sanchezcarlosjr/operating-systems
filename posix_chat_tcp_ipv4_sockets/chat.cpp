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

class Socket {
	public:
		int bufferSize=512;
		int maxOutstandingConnectionRequest=1;
		sa_family_t addressFamily=AF_INET;
		int socketDescriptor;
		int peerDescriptor;
		struct sockaddr_in socketAddress;
		int socketPort;
		struct sockaddr_in peerAddress;
		const char* peerAddressIPv4;
		bool isPassive = false;
		Socket() {}
		Socket(const char* peerAddressIPv4, const char* peerPort): peerAddressIPv4(peerAddressIPv4) {
			peerAddress = {
				.sin_family=addressFamily,
				.sin_port=htons(atoi(peerPort)),
				.sin_addr={
					.s_addr=0
				}
			};
			this->isPassive = true;
		}
};

class State {
public:
	virtual State* transite(Socket* s)  {
		return 0;
	}
	virtual bool isAcceptState() {
		return false;
	}
};


class Done: public State {
        State* transite(Socket* s) {
		printf("\rClosing communication...\n");
		close(s->peerDescriptor);
		close(s->socketDescriptor);

		printf("\rServer closes comunnication.\n");


		return this;
	}
	bool isAcceptState() {
		return true;
	}
};

class ActiveSocket: public State {
	State* transite(Socket* socket) {
		printf("\rReceiving message from client...\n");
		char buffer[BUFFER_SIZE];
		if (recv(socket->peerDescriptor, buffer, BUFFER_SIZE, 0) < 0) {
			printf("\r[ERROR] Reception failed.\n");
			exit(EXIT_FAILURE);
		}
		printf("\rMessage: %s\n", buffer);
		return new Done();
	}
};

class PassiveSocket: public State {
	State* transite(Socket* socket) {
		const char* message = "MESSAGE FROM PASSIVE";
		size_t messageLength = strlen(message);
		ssize_t numBytes = send(socket->socketDescriptor, message, messageLength, 0);
		if (numBytes < 0) {
			fputs("\r[ERROR] send failed\n", stderr);
			exit(EXIT_FAILURE);
		}
		if (numBytes != messageLength) {
			fputs("\r[ERROR] send un expected number of bytes\n", stderr);
			exit(EXIT_FAILURE);
		}
		return new Done();
	}
};

class BindingSocket: public State {
	State* transite(Socket* s) {
		s->socketAddress = {
			.sin_family=s->addressFamily,
			.sin_port=0,
			.sin_addr={
				.s_addr=htonl(INADDR_ANY)
			}
		};
		if (bind(s->socketDescriptor, (struct sockaddr*) &s->socketAddress, sizeof(s->socketAddress)) < 0) {
			printf("\r[ERROR] Socket bind failed.\n");
			exit(-1);
		}
		if (listen(s->socketDescriptor, MAX_OUTSTANDING_CONNECTION_REQUEST) < 0) {
			printf("\r[ERROR] Starting listening income connections failed.\n");
			exit(-1);
		}
		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		if (getsockname(s->socketDescriptor, (struct sockaddr *)&sin, &len) == -1) {
			perror("getsockname");
			exit(-1);
		}
		s->socketPort = ntohs(sin.sin_port);
		printf("\rListening on port %d\n", s->socketPort);

		s->peerAddress = *(new sockaddr_in);
		socklen_t peerAddressLength = sizeof(s->peerAddress);
		if ((s->peerDescriptor = accept(s->socketDescriptor, (struct sockaddr *) &s->peerAddress, &peerAddressLength)) < 0) {
			printf("\r[ERROR] accept() failed\n");
			exit(-1);
		}
		char peerName[INET_ADDRSTRLEN];
		if (inet_ntop(s->addressFamily, &s->peerAddress.sin_addr.s_addr, peerName, sizeof(peerName)) == NULL) {
			printf("\r[ERROR] Unable to client address\n");
			exit(-1);
		}
		return new ActiveSocket();
	}
};


class ConnectionSocket: public State {
	State* transite(Socket* s) {
		if (inet_pton(s->addressFamily, s->peerAddressIPv4, &s->peerAddress.sin_addr.s_addr) <= 0) {
			return new BindingSocket();
		}
		printf("\rConnecting to server...\n");
		if (connect(s->socketDescriptor, (struct sockaddr *) &s->peerAddress, sizeof(s->peerAddress)) < 0) {
			return new BindingSocket();
		}
		return new PassiveSocket();
	}
};

class SocketCreation: public State {
	public:
		State* transite(Socket* s) {
		        printf("\rCreating socket...\n");
			if ((s->socketDescriptor = socket(s->addressFamily, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				fputs("\r[ERROR] socket failed\n", stderr);
				exit(EXIT_FAILURE);
			}
			if (s->isPassive) {
				return new ConnectionSocket();
			}
			return new BindingSocket();
		}
};


class Machine {
private:
		State* currentState = new SocketCreation();
		Socket* socket;
public:
		Machine(Socket* socket): socket(socket) {
		}
		Socket* execute() {
			while(!(currentState = currentState->transite(socket))->isAcceptState());
			return socket;
		}
};


int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 1) {
	  printf("\rchat.out <PEER ADDRESS IPv4> <PEER PORT>\n");
	  return EXIT_SUCCESS;
  }
  Machine machine(argc == 1 ? new Socket() : new Socket(argv[1], argv[2]));
  machine.execute();
  return EXIT_SUCCESS;
}
