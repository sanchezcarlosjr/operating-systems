//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_MACHINE_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_MACHINE_H

#include "socket_state.h"
#include "socket.h"
#include "accepting_socket_state.h"
#include "notification.h"
#include "socket_state_creation.h"

class SocketMachine {
private:
		State* currentState = new SocketCreation();
		Socket* socket;
public:
		explicit SocketMachine(Socket* socket): socket(socket) {
		}
		Socket* execute() {
			while(!(currentState = currentState->transit(socket))->isAcceptState())
                ;
            currentState->transit(socket);
			return socket;
		}
};

#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_MACHINE_H
