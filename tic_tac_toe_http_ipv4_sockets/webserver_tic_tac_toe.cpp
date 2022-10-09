#include <string>
#include <iostream>
#include "sockets/socket.h"
#include "sockets/socket_machine.h"
#include <sstream>
#include <fstream>
#include <regex>
#include <vector>

std::regex regex_request("(GET|POST) (/(\\?row=([0-8])&column=([0-8]))?)");

class WebSocket : public Socket {
private:
	int players = 0;
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
	std::string req = receiveMessage();
	std::smatch matches;
	std::regex_search(req, matches, regex_request);
	std::vector<std::string> request;
	for (auto match: matches) {
		std::string m = match.str();
		if (m != "")
			request.push_back(m);
	}
	if(request[1] == "GET" && request.size() == 3) {
		view("index.html");
		players++;
	}
	if(players == 2) {
		players = 0;
	}
	if(request[1] == "GET" && request.size() == 6) {
		sendMessage("0,X,Y");
	}
    }

    void startPassive() {}
};

int main(int argc, char *argv[]) {
    if (argc != 1) {
        printf("\rwebserver_tic_tac_toe.out\n");
        return EXIT_SUCCESS;
    }
    SocketMachine machine(new WebSocket());
    machine.execute();
    return EXIT_SUCCESS;
}
