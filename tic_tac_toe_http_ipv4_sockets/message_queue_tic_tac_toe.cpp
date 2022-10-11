#include "tic_tac_toe.h"

int main(int argc, char* argv[]) {
	if (argc != 7) {
		std::cout << "[ERROR] We expect 7 parameters: SymbolPlayer1 QueueId UUIDPlayer1 SymbolP2 QueueId UUIDP2" << std::endl;
		return -1;
	}
	Game game(new ConsoleBoard, {
			new MessageQueuePlayer(argv[1][0], atoi(argv[2]),  std::string(argv[3]), 0), 
			new MessageQueuePlayer(argv[4][0], atoi(argv[5]), std::string(argv[6]), 1)
			}
		 );
	game.play();
	return 0;
}
