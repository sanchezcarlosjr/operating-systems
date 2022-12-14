#include <iostream>
#include <string>
#include "sockets/notification.h"
#include <vector>
#include <sstream>
#include <unistd.h>
#define FULL_BOARD -286331154

struct Coordinate {
	int row=-1;
	int column=-1;
	char symbol;
};

long int BOARD[3][3] = {{0x80080080, 0x40008000, 0x20000808}, {0x08040000, 0x04004044, 0x02000400}, {0x00820002, 0x00402000, 0x00200220}};

class Player {
	protected:
		long int board = 0x0;
		Coordinate coordinate{};
	public:
		explicit Player(char symbol=' ') {
			coordinate.symbol=symbol;
		}
		bool isPlayable(Player* player, Coordinate coordinate) {
			return ((board | player->board) & BOARD[coordinate.row][coordinate.column]) == 0;
		}
		bool isBoardFull(Player* player) {
			return (board | player->board) == FULL_BOARD;
		}
		void registerCoordinate(Coordinate coordinate) {
			this->coordinate = coordinate;
		}
		long int save() {
			board |= BOARD[coordinate.row][coordinate.column];
			return board;
		}
		Player* copy() {
			Player* player = new Player(coordinate.symbol);
			player->board = board;
			return player;
		}
		[[nodiscard]] bool isWinning() const {
			return (board & (board >> 1) & (board << 1)) != 0;
		}
		[[nodiscard]] bool hasWon() const {
			return (board & (board >> 1) & (board << 1)) != 0;
		}
		void start() {}
		virtual void lose() {}
		virtual void lose(Coordinate coordinate) {}
		virtual Coordinate win() {
			return coordinate;
		}
		virtual Coordinate tie() {
			return coordinate;
		}
		virtual Coordinate tie(Coordinate coordinate) {
			return coordinate;
		};
		virtual bool askIfContinueMatch() {
			return false;
		}
		virtual Coordinate move() {
			return coordinate;
		}
		virtual Coordinate move(Coordinate previousCoordinate) {
			return coordinate;
		}
};

#include <sys/msg.h>
#include <nlohmann/json.hpp>
#include <unistd.h>

struct Message {
	long type;
	char text[300];
} message;


class MessageQueuePlayer: public Player {
private:
	int msgid = -1;
	int turn = 0;
	std::string uuid;
public:
	MessageQueuePlayer(char symbol, int key, std::string uuid, int turn): Player(symbol), uuid(uuid), turn(turn) {
		msgid = msgget(key, 0666 | IPC_CREAT);
		if(msgid < 0) {
			std::cout << "[ERROR] msgid is not available" << std::endl;
			exit(-1);
		}
		sendState("7", coordinate);
		if(turn == 1) {
			sendState("3");
		}
	}
	void sendState(std::string state, Coordinate previousCoordinate={.row=-1,.column=-1}) {
		nlohmann::json j2;
		j2["state"] = state;
		j2["uuid"] = uuid;
		j2["symbol"] = std::string(1, previousCoordinate.symbol);
		j2["row"] = previousCoordinate.row;
		j2["column"] = previousCoordinate.column;
		std::string sjson = j2.dump();
		message.type = 2;
		strcpy(message.text, sjson.c_str());
		msgsnd(msgid, &message, sjson.size(), 0);
	}
	Coordinate win() override {
		sendState("4");
		return coordinate;
	}
	void lose(Coordinate previousCoordinate) override {
		sendState("6", previousCoordinate);
	}
	Coordinate tie() override {
		sendState("5");
		return coordinate;
	}
	bool askIfContinueMatch() override {
		return false;
	}
	Coordinate move(Coordinate previousCoordinate) override {	
		sendState("2", previousCoordinate);
		auto msg = Message();
		msgrcv(msgid, &msg, sizeof(msg), 3, 0);
		nlohmann::json j2 =  nlohmann::json::parse(msg.text);
		coordinate.row = j2["row"];
		coordinate.column = j2["column"];
		sendState("3", coordinate);
		return coordinate;
	}
};

#include <fstream>
class Computer: public Player {
	private:
		nlohmann::json data;
		std::string state = "0";
	public:
		Computer(char symbol): Player(symbol) {
			std::ifstream f("public/minimax.json");
			data = nlohmann::json::parse(f);
		}
		Coordinate move(Coordinate previousCoordinate) override {
			state = data[state][std::to_string(previousCoordinate.row*10+previousCoordinate.column)];
			state = data[state]["best"];
			coordinate.row = data[state]["R"]["row"];
			coordinate.column = data[state]["R"]["column"];
			return coordinate;
		}
};

class LocalConsolePlayer: public Player {
	public:	
		explicit LocalConsolePlayer(char symbol): Player(symbol) {}
		Coordinate win() override {
			std::cout << std::endl << "You've won";
			return coordinate;
		}
		Coordinate tie() override {
			std::cout << std::endl << "You tied";
			return coordinate;
		}
		bool askIfContinueMatch() override {
			std::cout << std::endl << "Do you want continue? (y/n) ";
			char response;
			std::cin >> response;
			return response == 'y';
		}
		Coordinate move() override {
			std::cout << "\rMove row column:";
			std::cin >> coordinate.row >> coordinate.column;
			return coordinate;
		}
		void lose() override {}
};

class ActiveConsolePlayer: public LocalConsolePlayer {
	private:
		Notification* notification;
	public:	
		ActiveConsolePlayer(char symbol, Notification* notification): LocalConsolePlayer(symbol), notification(notification) {}	
		Coordinate move() override {
			std::cout << "Move row column where 0<=row<=2 and 0<=column<=2:";
			std::cin >> coordinate.row >> coordinate.column;
			std::cout << "\n";
			std::stringstream ss;
			ss << coordinate.row << " " << coordinate.column;
			notification->sendMessage(ss.str());
			return coordinate;
		}
		void lose() override {}
};


class PassiveConsolePlayer: public LocalConsolePlayer {
	private:
		Notification* notification;
	public:	
		PassiveConsolePlayer(char symbol, Notification* notification): LocalConsolePlayer(symbol), notification(notification) {}
		Coordinate win() override {
			std::cout << "You've lost!" <<std::endl;
			return coordinate;
		}
		Coordinate move() override {
			std::cout << "Wait your turn!" << std::endl << std::endl;
			std::stringstream ss(notification->receiveMessage());
			ss >> coordinate.row >> coordinate.column;
			return coordinate;
		}
		void lose() override {}
};

#define EMPTY_COORDINATE ' '
#define X 'x'
#define O 'o'

#define WIN  1
#define ERROR (-1)
#define NEXT 2


class ConsoleBoard {
	private:
		char board[3][3];
		Coordinate coordinate;
	public:
		void reset() {
			for(auto & row : board)
				for(char & column : row)
					column = EMPTY_COORDINATE;
		}
		int save(Player* player) {
			coordinate = player->move(coordinate);
			if (0 > coordinate.row || 0 > coordinate.column  || coordinate.row+coordinate.column > 4  || board[coordinate.row][coordinate.column] != EMPTY_COORDINATE)
				return ERROR;
			player->save();
			board[coordinate.row][coordinate.column] = coordinate.symbol;
			if (player->isWinning())
				return WIN;
			return NEXT;
		}
		void draw() {
			for(auto & row : board) {
				for(int column=0; column<3; column++)
					std::cout<< row[column] << (column < 2 ? '|': EMPTY_COORDINATE);
				std::cout << std::endl << "-----" << std::endl;
			}
			std::cout << std::endl;
		}
		int size() {
			return 9;
		}
};

#define RESET 0 
#define NEXT_TURN 1
#define TRY_AGAIN 2
#define NEW_PLAYER 3
#define MAX_PLAYER 4
#define NEW_GAME 5
#define DONE 6
#define NO_PLAYER 7

#define PLAYERS 2

class Game {
	private:
		ConsoleBoard* board;
		std::vector<Player*> players;
		int turn = 0;
		Player* currentPlayer() {
			return players[turn%PLAYERS];
		}
		
		int transit(int state) {
			if(players.size() != 2) {
				exit(-1);
			}
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
				Coordinate coordinate = currentPlayer()->win();
				turn++;
				currentPlayer()->lose(coordinate);
				return NEW_GAME;
			}
			bool isTie = ++turn == board->size();
			if (isTie) {
				Coordinate coordinate = currentPlayer()->tie();
				turn++;	
				currentPlayer()->tie(coordinate);
				return NEW_GAME;
			}
			return NEXT_TURN;
		}
	public:
		Game(ConsoleBoard* board, std::vector<Player*> players): board(board), players(players){}
		void play() {
			int state = RESET;
			while((state = transit(state)) != DONE);
		}
};
