#include "AI.hpp"

AI::AI() : _aiTeam(WHITE), _opponentTeam(BLACK) {}

AI::AI(e_stone aiTeam) : _aiTeam(aiTeam) {
	this->_opponentTeam = (aiTeam == BLACK) ? WHITE : BLACK;
}

AI::AI(const AI &other) {
	*this = other;
}

AI &AI::operator=(const AI &other) {
	if (this != &other) {
		this->_aiTeam = other._aiTeam;
		this->_opponentTeam = other._opponentTeam;
	}
	return *this;
}

AI::~AI() {}

Move AI::getBestMove(const Board &board) {
	(void)board;

	Move bestMove;
	bestMove.x = 9;
	bestMove.y = 9;

	//algo minimax goes here

	return bestMove;
}

//returns a score depending on what the line contains
int AI::_evaluateLine(int count, int openEnds, bool isAi) const {
	int score = 0;
	if (count >= 5) {
		score = 100000; //5 stones
	} else if (count == 4) {
		if (openEnds == 2)
			score = 10000; //4 stones, 2 open ends
		else if (openEnds == 1)
			score = 1000; //4 stones, 1 open end
	} else if (count == 3) {
		if (openEnds == 2)
			score = 500; //3 stones, 2 open ends
		else if (openEnds == 1)
			score = 100; //3 stones, 1 open end
	} else if (count == 2) {
		if (openEnds == 2)
			score = 50; //2 stones, 2 open ends
		else if (openEnds == 1)
			score = 10; //2 stones, 1 open end
	}
	return isAi ? score : -score; //-score if its the opponent (humain) alignements
}

int AI::_evaluateBoard(const Board &board) const {
	int totalScore = 0;
	int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {-1, 1}};

	for (int y = 0; y < 19; ++y) {
		for (int x = 0; x < 19; ++x) {
			e_stone currentStone = board.getStone(x, y);

			if (currentStone == EMPTY)
				continue; //empty case are useless
			
			bool isAi = (currentStone == this->_aiTeam);

			//checking 4 axes around this stone
			for (int d = 0; d < 4; ++d) {
				int dx = directions[d][0];
				int dy = directions[d][1];

				//to provide from counting twice a stone
				int prevX = x - dx;
				int prevY = y - dy;
				//if previous stone is on board and same team, ignoring it
				if (prevX >= 0 && prevX < 19 && prevY >= 0 && prevY < 19) {
					if (board.getStone(prevX, prevY) == currentStone) {
						continue;
					}
				}

				int count = 1;
				int openEnds = 0;

				//looking backward to see if it's empty
				if (prevX >= 0 && prevX < 19 && prevY >= 0 && prevY < 19) {
					if (board.getStone(prevX, prevY) == EMPTY) {
						openEnds++;
					}
				}

				//counting stones forward
				int nx = x + dx;
				int ny = y + dy;
				while (nx >= 0 && nx < 19 && ny >= 0 && ny < 19 && board.getStone(nx, ny) == currentStone) {
					count++;
					nx += dx;
					ny += dy;
				}

				//searching open end forward
				if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19) {
					if (board.getStone(nx, ny) == EMPTY) {
						openEnds++;
					}
				}

				totalScore += this->_evaluateLine(count, openEnds, isAi);
			}
		}
	}
	//adding captures score
	totalScore += (board.getCaptures(this->_aiTeam) * 2000);
	totalScore -= (board.getCaptures(this->_opponentTeam) * 2000);

	return totalScore;
}