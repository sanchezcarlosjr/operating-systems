//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_KIND_SOCKET_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_KIND_SOCKET_STATE_H

#include "socket_state.h"
#include "socket.h"
#include "accepting_socket_state.h"
#include "notification.h"
#include <thread>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ifaddrs.h>
#include <cstdio>

class KindSocket: public State {
public:
	State* transit(Socket* socket) override {
		return new Done();
	}
	virtual int to(Socket* socket) = 0;
	virtual int from(Socket* socket) = 0;
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_KIND_SOCKET_STATE_H
