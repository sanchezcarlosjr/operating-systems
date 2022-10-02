#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_BINDING_SOCKET_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_BINDING_SOCKET_STATE_H

#include "socket_state.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ifaddrs.h>
#include <cstdio>
#pragma once

// https://stackoverflow.com/questions/212528/how-can-i-get-the-ip-address-of-a-linux-machine
void showIpAddress() {
    struct ifaddrs * ifAddrStruct=nullptr;
    struct ifaddrs * ifa=nullptr;
    void * tmpAddrPtr=nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("\r%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=nullptr) freeifaddrs(ifAddrStruct);
}

class BindingSocket: public State {
	State* transit(Socket* s) override {
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
		return new AcceptingSocketState();
	}
};



#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_BINDING_SOCKET_STATE_H
