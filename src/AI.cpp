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

//heuristic, give score to the actual board
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

//checks if an empty cell has neighbor at a given distance
bool	AI::_hasNeighbor(const Board &board, int x, int y, int distance) const {
	for (int dy = -distance; dy <= distance; ++dy) {
		for (int dx = -distance; dx <= distance; ++dx) {
			if (dx == 0 && dy == 0)
			continue; //stone itself
		
		int nx = x + dx;
		int ny = y + dy;
		
		if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19 ) {
			if (board.getStone(nx, ny) != EMPTY) {
				return true; //found a neighbor
			}
		}
	}
}
return false; //cell has absolutely no neighbor stones all around
}

//scan the board and return list of moves (within all stones neighbors)
std::vector<Move>	AI::_generateMoves(const Board &board) const {
	std::vector<Move> moves;
	bool isEmptyBoard = true;
	
	for (int y = 0; y < 19; ++y) {
		for (int x = 0; x < 19; ++x) {
			if (board.getStone(x, y) != EMPTY) {
				isEmptyBoard = false;
			} else {
				if (this->_hasNeighbor(board, x, y, 2)) {
					Move m;
					m.x = x;
					m.y = y;
					moves.push_back(m);
				}
			}
		}
	}
	
	//if AI is playing the first move on the board
	if (isEmptyBoard) {
		Move m;
		m.x = 9;
		m.y = 9;
		moves.push_back(m);
	}
	return moves;
}

//mimimax algo with alpha beta pruning
int AI::_minimax(Board board, int depth, int alpha, int beta, bool isMaximizing) {
	//if we reach max depth we stop recursing
	if (depth == 0)
		return this->_evaluateBoard(board);

	//list of possible moves
	std::vector<Move> moves = this->_generateMoves(board);

	if (moves.empty())
		return this->_evaluateBoard(board);

	if (isMaximizing) {
		//AI's turn, searching for highest value (alpha)
		int maxEval = -2000000; //our -infinity for int_min memory security

		for (size_t i = 0; i < moves.size(); ++i) {
			Board nextBoard = board; //copying the board
			//playing the simulated move
			nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);
			nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);

			//recursive, getting deeper, with the opponent (humain) move
			int eval = this->_minimax(nextBoard, depth - 1, alpha, beta, false);

			if (eval > maxEval)
				maxEval = eval; //this move gave more pts that seen until now
			if (eval > alpha)
				alpha = eval; //this branch got the higher nb of pts

			if (beta <= alpha)
				break; //if humain has a better move, pruning
		}
		return maxEval;
	} else {
		//humain's turn,searching for lowest value (beta)
		int minEval = 2000000; //our +infinity for int_max memory security

		for (size_t i = 0; i < moves.size(); ++i) {
			Board nextBoard = board; //copying the board
			//playing the simulated move
			nextBoard.setStone(moves[i].x, moves[i].y, this->_opponentTeam);
			nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_opponentTeam);

			//recursive, getting deeper, with the opponent (AI) move
			int eval = this->_minimax(nextBoard, depth - 1, alpha, beta, true);

			if (eval < minEval)
				minEval = eval; //this move gave less pts that seen until now
			if (eval < beta)
				beta = eval; //this branch got the lowest nb of pts

			if (beta <= alpha)
				break; //if AI has better move elsewhere, won't go with this branch
		}
		return minEval;
	}
}

//return coordinates of the best possible move
Move AI::getBestMove(const Board &board) {
	std::vector<Move> moves = this->_generateMoves(board);
	
	if (moves.empty()) {
		Move bestMove;
		bestMove.x = -1;
		bestMove.y = -1; //if board is full
		return bestMove;
	}

	if (moves.size() == 1) {
		return moves[0]; //if empty board, play middle without losing time
	}

	Move bestMove;
	bestMove.x = -1;
	bestMove.y = -1;

	int bestScore = -2000000;
	int alpha = -2000000;
	int beta = 2000000;
	int depth = 3; //should be 10, but is 3 for now, so my computer doesn't crash

	for (size_t i = 0; i < moves.size(); ++i) {
		Board nextBoard = board;
		nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);
		nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);

		//launching minimax with next move being the humain one
		int score = this->_minimax(nextBoard, depth - 1, alpha, beta, false);

		if (score > bestScore) {
			bestScore = score;
			bestMove = moves[i]; //keeping the coordinates of the new move that got more pts
		}
		if (score > alpha)
			alpha = score;
	}

	return bestMove;
}
