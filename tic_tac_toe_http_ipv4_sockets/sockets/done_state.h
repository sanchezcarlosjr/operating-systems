//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_DONE_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_DONE_STATE_H

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

class Done: public State {
        State* transit(Socket* socket) override {
		printf("\rClosing communication...\n");
		printf("\rServer closes communication.\n");
		return this;
	}
	bool isAcceptState() override {
		return true;
	}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_DONE_STATE_H
