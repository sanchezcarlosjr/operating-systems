#include <string>
#include <iostream>
#include "sockets/socket.h"
#include "sockets/socket_machine.h"
#include<sstream>
#include <fstream>

class WebSocket : public Socket {
public:
    bool keepLiveSessionAfterDone = true;

    WebSocket() : Socket() {}

    WebSocket(char *ip, char *port) : Socket(ip, port) {}

    void view(std::string fileName) {
        std::string html = "HTTP/1.1 200 OK\nServer:sanchezcarlosjr\nContent-Type:text/html\nContent-Length: ";
        std::ifstream file;
        file.open(fileName);
        if (!file) {
            std::cout << "[ERROR] Unable to read file!";
            exit(EXIT_FAILURE);
        }
        std::ostringstream fileStream;
        fileStream << file.rdbuf();
        std::string content = fileStream.str();
        std::ostringstream ss;
        ss << content.length() << "\r\n\r\n" << content;
        html.append(ss.str());
        sendMessage(html);
        file.close();
    }

    void startActive() {
        view("index.html");
    }

    void startPassive() {}
};

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 1) {
        printf("\rwebserver_tic_tac_toe.out <IP> <PORT>\n");
        return EXIT_SUCCESS;
    }
    SocketMachine machine(argc == 1 ? new WebSocket() : new WebSocket(argv[1], argv[2]));
    machine.execute();
    return EXIT_SUCCESS;
}
