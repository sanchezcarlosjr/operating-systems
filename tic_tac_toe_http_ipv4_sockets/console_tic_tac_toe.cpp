#include "tic_tac_toe.h"

int main() {
	Game game(new ConsoleBoard, {new LocalConsolePlayer(X), new LocalConsolePlayer(O)});
	game.play();
}
