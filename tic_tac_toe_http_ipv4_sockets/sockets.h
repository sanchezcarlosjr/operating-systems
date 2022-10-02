#include <stdio.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include "tic_tac_toe.h"

#define MAX_OUTSTANDING_CONNECTION_REQUEST 1
#define BUFFER_SIZE 512

class Socket: public Notification {
	public:
		int bufferSize=512;
		int maxOutstandingConnectionRequest=1;
		sa_family_t addressFamily=AF_INET;
		int socketDescriptor;
		int peerDescriptor;
		struct sockaddr_in socketAddress;
		int socketPort;
		struct sockaddr_in peerAddress;
		char peerName[INET_ADDRSTRLEN];
		const char* peerAddressIPv4;
		bool isPassive = false;
		int to;
		int from;
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
		char* receiveMessage() {
			char* buffer;
			ssize_t numBytes = 0;
			if ((numBytes = recv(from, buffer, BUFFER_SIZE-1, 0)) < 0) {
				printf("\r[ERROR] Reception failed.\n");
				exit(EXIT_FAILURE);
			}
			buffer[numBytes] = '\0';
			return buffer;
		}
		void sendMessage(const char* message) {
			size_t messageLength = strlen(message);
			ssize_t numBytes = send(to, message, messageLength, 0);
			if (numBytes < 0) {
				fputs("\r[ERROR] send failed\n", stderr);
				exit(EXIT_FAILURE);
			}
			if (numBytes != messageLength) {
				fputs("\r[ERROR] send unexpected number of bytes.\n", stderr);
				exit(EXIT_FAILURE);
				}
		}
		bool isDone() {
			return false;
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
		printf("\rServer closes comunnication.\n");
		return this;
	}
	bool isAcceptState() {
		return true;
	}
};

class KindSocket: public State {
public:
	virtual State* transite(Socket* socket) {
		return new Done();
	}
	virtual int to(Socket* socket) = 0;
	virtual int from(Socket* socket) = 0;
};

class ActiveSocket: public KindSocket {
public:
	virtual State* transite(Socket* socket) {	
		printf("\rYou have connected succesful to %s:%d\n", socket->peerName, ntohs(socket->peerAddress.sin_port));
		socket->to = to(socket);
		socket->from = from(socket);
		Game game(new ConsoleBoard, {new ActiveConsolePlayer(X, socket), new PasiveConsolePlayer(O, socket)});
		game.play();
		return new Done();
	}
	int to(Socket* socket) {
		return socket->peerDescriptor;
	}
	int from(Socket* socket) {
		return socket->peerDescriptor;
	}
};

class PassiveSocket: public KindSocket {
public:
	virtual State* transite(Socket* socket) {	
		printf("\rYou have connected succesful to %s:%d\n", socket->peerName, ntohs(socket->peerAddress.sin_port));
		socket->to = to(socket);
		socket->from = from(socket);
		Game game(new ConsoleBoard, {new PasiveConsolePlayer(X, socket), new ActiveConsolePlayer(O, socket)});
		game.play();
		return new Done();
	}
	int to(Socket* socket) {
		return socket->socketDescriptor;
	}
	int from(Socket* socket) {
		return socket->socketDescriptor;
	}
};

// https://stackoverflow.com/questions/212528/how-can-i-get-the-ip-address-of-a-linux-machine
void showIpAddress() {
	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
							   // is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			printf("\r%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
								   // is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//printf("\r%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
		} 
	}
	if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
}

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
			exit(EXIT_FAILURE);
		}
		if (listen(s->socketDescriptor, MAX_OUTSTANDING_CONNECTION_REQUEST) < 0) {
			printf("\r[ERROR] Starting listening income connections failed.\n");
			exit(EXIT_FAILURE);
		}
		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		if (getsockname(s->socketDescriptor, (struct sockaddr *)&sin, &len) == -1) {
			perror("getsockname");
			printf("\r[ERROR] SO cannot find an available port\n");
			exit(EXIT_FAILURE);
		}
		s->socketPort = ntohs(sin.sin_port);
		showIpAddress();
		printf("\rListening on port %d\n", s->socketPort);

		s->peerAddress = *(new sockaddr_in);
		socklen_t peerAddressLength = sizeof(s->peerAddress);
		if ((s->peerDescriptor = accept(s->socketDescriptor, (struct sockaddr *) &s->peerAddress, &peerAddressLength)) < 0) {
			printf("\r[ERROR] accept() failed\n");
			exit(EXIT_FAILURE);
		}
		if (inet_ntop(s->addressFamily, &s->peerAddress.sin_addr.s_addr, s->peerName, sizeof(s->peerName)) == NULL) {
			printf("\r[ERROR] Unable to client address\n");
			exit(EXIT_FAILURE);
		}
		return new ActiveSocket();
	}
};


class ConnectionSocket: public State {
	State* transite(Socket* s) {
		if (inet_pton(s->addressFamily, s->peerAddressIPv4, &s->peerAddress.sin_addr.s_addr) <= 0) {
			printf("\rConnection failed. We are a server.\n");
			return new BindingSocket();
		}
		printf("\rConnecting to server...\n");
		if (connect(s->socketDescriptor, (struct sockaddr *) &s->peerAddress, sizeof(s->peerAddress)) < 0) {
			printf("\rConnection failed. We are a server.\n");
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
			currentState->transite(socket);
			return socket;
		}
};
