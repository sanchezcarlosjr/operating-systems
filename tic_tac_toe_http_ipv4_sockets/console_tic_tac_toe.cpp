#include <iostream>
#include <string>
#include <stdlib.h>

struct Coordinate {
	int row;
	int column;
	char symbol;
};

long int BOARD[3][3] = {{0x80080080, 0x40008000, 0x20000808}, {0x08040000, 0x04004044, 0x02000400}, {0x00820002, 0x00402000, 0x00200220}};

class Player {
	private:
		Coordinate coordinate;
		long board = 0;
	public:
		Player(char symbol=' ') {
			coordinate.symbol=symbol;
		}
		void win() {
			std::cout << std::endl << "You've won";
		}
		void tie() {
			std::cout << std::endl << "You tied";
		}
		bool askIfContinueMatch() {
			std::cout << std::endl << "Do you want continue? (y/n) ";
			char response;
			std::cin >> response;
			return response == 'y';
		}
		void save() {
			board |= BOARD[coordinate.row][coordinate.column];
		}
		bool isWinning() {
			return (board & (board >> 1) & (board << 1)) != 0;
		}
		Coordinate move() {
			std::cout << "\rMove x y:";
			std::cin >> coordinate.row >> coordinate.column;
			return coordinate;
		}
};

#define EMPTY_COORDINATE ' '
#define X 'x'
#define O 'o'

#define WIN  1
#define ERROR -1
#define NEXT 2


class ConsoleBoard {
	private:
		char board[3][3];
	public:
		void reset() {
			for(int row=0; row<3; row++)
				for(int column=0; column<3; column++)
					board[row][column] = EMPTY_COORDINATE;
		}
		int save(Player* player) {
			Coordinate coordinate = player->move();
			if (0 > coordinate.row || 0 > coordinate.column  || coordinate.row+coordinate.column > 4  || board[coordinate.row][coordinate.column] != EMPTY_COORDINATE)
				return ERROR;
			player->save();
			board[coordinate.row][coordinate.column] = coordinate.symbol;
			if (player->isWinning())
				return WIN;
			return NEXT;
		}
		void draw() {
			system("clear");
			for(int row=0; row<3; row++) {
				for(int column=0; column<3; column++)
					std::cout<< board[row][column] << (column < 2 ? '|': EMPTY_COORDINATE);
				std::cout << std::endl << "-----" << std::endl;
			}
		}
		int size() {
			return 9;
		}
};

#define RESET 0 
#define NEXT_TURN 1
#define TRY_AGAIN 2
#define NEW_GAME 5
#define DONE 6

#define PLAYERS 2

class Game {
	private:
		ConsoleBoard* board;
		Player* players[PLAYERS] = {new Player(X), new Player(O)};
		int turn = 0;
		Player* currentPlayer() {
			return players[turn%PLAYERS];
		}
		int transite(int state) {
			if(state == NEW_GAME) {
				return currentPlayer()->askIfContinueMatch() ? RESET : DONE;
			}
			if(state == RESET) {
				board->reset();
				board->draw();
				turn = 0;
				return NEXT_TURN;
			}
			int result = 0;
			if ((result = board->save(currentPlayer())) == ERROR)
				return TRY_AGAIN;
			board->draw();
			if(result == WIN) {
				currentPlayer()->win();
				return NEW_GAME;
			}
			bool isTie = ++turn == board->size();
			if (isTie) {
				currentPlayer()->tie();
				return NEW_GAME;
			}
			return NEXT_TURN;
		}
	public:
		Game(ConsoleBoard* board): board(board){}
		void play() {
			int state = RESET;
			while((state = transite(state)) != DONE);
		}
};

int main() {
	Game game(new ConsoleBoard);
	game.play();
}