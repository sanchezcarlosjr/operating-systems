//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_CREATION_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_CREATION_H

#include "socket_state.h"
#include "socket.h"
#include "accepting_socket_state.h"
#include "notification.h"
#include "connection_socket_state.h"
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

class SocketCreation: public State {
	public:
		State* transit(Socket* s) override {
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

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_CREATION_H
