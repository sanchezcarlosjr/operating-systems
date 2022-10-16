#include "tic_tac_toe.h"

int main(int argc, char* argv[]) {
	if (argc != 9) {
		std::cout << "[ERROR] We expect 9 parameters: Human|Computer SymbolPlayer1 QueueId UUIDPlayer1 Human|Computer SymbolP2 QueueId UUIDP2" << std::endl;
		return -1;
	}
	Game game(new ConsoleBoard, {
			argv[1][0] == 'H' ? (Player*) new MessageQueuePlayer(argv[2][0], atoi(argv[3]),  std::string(argv[4]), 0) : new Computer(argv[2][0]),
			argv[5][0] == 'H' ? (Player*) new MessageQueuePlayer(argv[6][0], atoi(argv[7]), std::string(argv[8]), 1) : new Computer(argv[6][0])
			}
		 );
	game.play();
	return 0;
}
