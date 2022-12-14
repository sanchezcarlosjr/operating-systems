#include "tic_tac_toe.h"
#include "sockets/socket.h"
#include "sockets/socket_machine.h"

class GameSocket: public Socket {
	public:
		GameSocket(): Socket() {}
		GameSocket(char* ip, char* port): Socket(ip, port) {}
		void startActive() override {
			Game game(new ConsoleBoard, {new ActiveConsolePlayer(X, this), new PassiveConsolePlayer(O, this)});
			game.play();
		}
		void startPassive() override {
			Game game(new ConsoleBoard, {new PassiveConsolePlayer(X, this), new ActiveConsolePlayer(O, this)});
			game.play();
		}
};


int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 1) {
	  printf("\rtic_tac_toe.out <PEER ADDRESS IPv4> <PEER PORT>\n");
	  return EXIT_SUCCESS;
  }
  SocketMachine machine(argc == 1 ? new GameSocket() : new GameSocket(argv[1], argv[2]));
  machine.execute();
  return EXIT_SUCCESS;
}
