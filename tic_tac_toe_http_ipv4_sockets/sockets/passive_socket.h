#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_PASSIVE_SOCKET_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_PASSIVE_SOCKET_H
#pragma once

#include "socket.h"
#include "socket_state.h"
#include "done_state.h"
#include "kind_socket_state.h"

class PassiveSocket: public KindSocket {
public:
	State* transit(Socket* socket) override {
		printf("\rYou have connected successful to %s:%d\n", socket->peerName, ntohs(socket->peerAddress.sin_port));
		socket->to = to(socket);
		socket->from = from(socket);
        socket->startPassive();
        close(socket->socketDescriptor);
		return new Done();
	}
	int to(Socket* socket) override {
		return socket->socketDescriptor;
	}
	int from(Socket* socket) override {
		return socket->socketDescriptor;
	}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_PASSIVE_SOCKET_H
