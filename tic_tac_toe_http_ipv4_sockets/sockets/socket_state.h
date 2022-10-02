#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_H

#pragma once

#include "socket.h"

class State {
public:
	virtual State* transit(Socket* s)  {
		return nullptr;
	}
	virtual bool isAcceptState() {
		return false;
	}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_STATE_H
