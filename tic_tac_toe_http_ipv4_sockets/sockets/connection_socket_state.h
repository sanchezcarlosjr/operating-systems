//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_CONNECTION_SOCKET_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_CONNECTION_SOCKET_STATE_H

#include "socket_state.h"
#include "socket.h"
#include "accepting_socket_state.h"
#include "notification.h"
#include "binding_socket_state.h"
#include "passive_socket.h"
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


class ConnectionSocket: public State {
	State* transit(Socket* s) override {
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

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_CONNECTION_SOCKET_STATE_H
