#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACCEPTING_SOCKET_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACCEPTING_SOCKET_STATE_H

#include "active_socket.h"
#include "socket_state.h"
#include <cstdio>
#include <ifaddrs.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#pragma once
class AcceptingSocketState: public State {
	State* transit(Socket* s) override {
		s->peerAddress = *(new sockaddr_in);
		socklen_t peerAddressLength = sizeof(s->peerAddress);
		if ((s->peerDescriptor = accept(s->socketDescriptor, (struct sockaddr *) &s->peerAddress, &peerAddressLength)) < 0) {
			printf("\r[ERROR] accept() failed\n");
			exit(EXIT_FAILURE);
		}
		if (inet_ntop(s->addressFamily, &s->peerAddress.sin_addr.s_addr, s->peerName, sizeof(s->peerName)) == nullptr) {
			printf("\r[ERROR] Unable to client address\n");
			exit(EXIT_FAILURE);
		}
        if(s->keepLiveSessionAfterDone) {
            (new ActiveSocket)->transit(s);
            return this;
        }
		return new ActiveSocket();
	}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACCEPTING_SOCKET_STATE_H
