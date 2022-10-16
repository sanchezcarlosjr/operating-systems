#include <nlohmann/json.hpp>
#include <fstream>
#include <list>
#include <limits>
#include "tic_tac_toe.h"
#include <algorithm>

nlohmann::json tree;
long int newStates = 0;

class Board {
	private:
		Player* p1;
		Player* p2;
		int turn = 0;
	public:
		Board(Player* p1=new Player('X'), Player* p2=new Player('O'), int turn=0): p1(p1), p2(p2), turn(turn) {}
		bool isLeave() {
			return p1->hasWon() || p2->hasWon() || p1->isBoardFull(p2);
		}
		int evaluate() {
			return p1->hasWon() ? 10 : p2->hasWon() ? -10 : 0;
		}
		bool isPlayable(Coordinate coordinate) {
			return p1->isPlayable(p2, coordinate);
		}
		std::string save(Coordinate coordinate, std::string currentState) {
			Player* p = turn == 1 ? p1 : p2;
			p->registerCoordinate(coordinate);
			tree[currentState]["board"] = p->save();
			tree[currentState]["turn"] = turn;
			return std::to_string(coordinate.row*10+coordinate.column);
		}
		Board* copy() {
			return new Board(p1->copy(), p2->copy(), 1-turn);
		}
};

class AdversarialState {
	private:
		long int state = 0;
		Board* board;
	public:
		AdversarialState(long int state=0, Board* board=new Board): board(board), state(state) {}
		int minimax() {	
			tree[currentState()]["R"]["row"] = -1;
			tree[currentState()]["R"]["column"] = -1;
			return minimax(true);
		}
		int minimax(bool isMaximizing) {
			if(board->isLeave())
				return board->evaluate();
			std::vector<AdversarialState*> adversarialStates = explore();
			if(isMaximizing) {
				int bestUtility = -std::numeric_limits<int>::infinity();
				for(auto adversarialState: adversarialStates) {
					int utility = adversarialState->minimax(false); 
					int previousUtility = bestUtility;
					bestUtility = std::max(bestUtility, utility);
					if(previousUtility < bestUtility)
					   tree[currentState()]["best"] = adversarialState->currentState();
				}
				tree[currentState()]["utility"] = bestUtility;
				return bestUtility;
			}
			int bestUtility = std::numeric_limits<int>::infinity();
			for(auto adversarialState: adversarialStates) {
				int utility = adversarialState->minimax(true); 
				int previousUtility = bestUtility;
				bestUtility = std::min(bestUtility, utility);
				if(previousUtility > bestUtility)
					   tree[currentState()]["best"] = adversarialState->currentState();
			}
			tree[currentState()]["utility"] = bestUtility;
			return bestUtility;
		}
		std::string currentState() {
			return std::to_string(state);
		}
		std::vector<AdversarialState*> explore() {	
			std::vector<AdversarialState*> adversarialStates; 
			for(int i=0; i<3; i++) 
				for(int j=0; j<3; j++) {
					Coordinate coordinate = {.row=i,.column=j};
					if(board->isPlayable(coordinate)) {
						newStates++;
						Board* board = this->board->copy();
						AdversarialState* adversarialState = new AdversarialState(newStates, board);
						std::string scoordinate = board->save(coordinate, adversarialState->currentState());
						tree[currentState()][scoordinate] = adversarialState->currentState();
						tree[adversarialState->currentState()]["R"]["row"] = coordinate.row;
						tree[adversarialState->currentState()]["R"]["column"] = coordinate.column;
						adversarialStates.push_back(adversarialState);
					}
				}
			return adversarialStates;
		}
};


int main() {
	AdversarialState adversarialState;
	adversarialState.minimax();
	std::ofstream o("minimax.json");
	o << std::setw(4) << tree << std::endl;
	return 0;
}
