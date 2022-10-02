//
// Created by sanchezcarlosjr on 10/2/22.
//

#ifndef TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_H
#define TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_H
#define MAX_OUTSTANDING_CONNECTION_REQUEST 4
#define BUFFER_SIZE 0xFFFFFFF

#include <netinet/in.h>
#include "notification.h"

#pragma once

class Socket: public Notification {
public:
    int bufferSize=BUFFER_SIZE;
    int maxOutstandingConnectionRequest=MAX_OUTSTANDING_CONNECTION_REQUEST;
    sa_family_t addressFamily=AF_INET;
    int socketDescriptor{};
    int peerDescriptor{};
    struct sockaddr_in socketAddress{};
    int socketPort{};
    struct sockaddr_in peerAddress{};
    char peerName[INET_ADDRSTRLEN]{};
    const char* peerAddressIPv4{};
    bool isPassive = false;
    bool keepLiveSessionAfterDone = true;
    int to{};
    int from{};
    Socket() = default;
    Socket(const char* peerAddressIPv4, const char* peerPort): peerAddressIPv4(peerAddressIPv4) {
        peerAddress = {
                .sin_family=addressFamily,
                .sin_port=htons(atoi(peerPort)),
                .sin_addr={
                        .s_addr=0
                }
        };
        this->isPassive = true;
    }
    std::string receiveMessage() override {
        char buffer[BUFFER_SIZE];
        std::string message;
        ssize_t numBytes = 0;
        if((numBytes = recv(from, buffer, BUFFER_SIZE-1, 0)) > 0)
            message.append(buffer, numBytes);
        if (numBytes < 0) {
            fputs("\r[ERROR] receive failed\n", stderr);
            exit(EXIT_FAILURE);
        }
        return message;
    }
    void sendMessage(std::string message) override {
        ssize_t numBytes = send(to, message.data(), message.length(), 0);
        if (numBytes < 0) {
            fputs("\r[ERROR] send failed\n", stderr);
            exit(EXIT_FAILURE);
        }
        if (numBytes != message.length()) {
            fputs("\r[ERROR] send unexpected number of bytes.\n", stderr);
            exit(EXIT_FAILURE);
        }
    }
    virtual void startActive() = 0;
    virtual void startPassive() = 0;
};
#endif //TIC_TAC_TOE_HTTP_IPV4_SOCKETS_SOCKET_H
