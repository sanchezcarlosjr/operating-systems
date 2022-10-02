#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACTIVE_SOCKET_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACTIVE_SOCKET_H
#pragma once

#include "socket_state.h"
#include "done_state.h"
#include "socket.h"
#include "kind_socket_state.h"

class ActiveSocket: public KindSocket {
public:
	State* transit(Socket* socket) override {
		printf("\rYou have connected successful to %s:%d\n", socket->peerName, ntohs(socket->peerAddress.sin_port));
		socket->to = to(socket);
		socket->from = from(socket);
        socket->startActive();
        close(socket->peerDescriptor);
		return new Done();
	}
	int to(Socket* socket) override {
		return socket->peerDescriptor;
	}
	int from(Socket* socket) override {
		return socket->peerDescriptor;
	}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_ACTIVE_SOCKET_H
