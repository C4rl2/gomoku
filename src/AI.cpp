#include "AI.hpp"

AI::AI() : _aiTeam(WHITE), _opponentTeam(BLACK), _depth(5) {}

AI::AI(e_stone aiTeam) : _aiTeam(aiTeam), _depth(5) {
	this->_opponentTeam = (aiTeam == BLACK) ? WHITE : BLACK;
}

AI::AI(const AI &other) {
	*this = other;
}

AI &AI::operator=(const AI &other) {
	if (this != &other) {
		this->_aiTeam = other._aiTeam;
		this->_opponentTeam = other._opponentTeam;
		this->_depth        = other._depth;
	}
	return *this;
}

AI::~AI() {}

void AI::setDepth(int depth) {
	this->_depth = depth;
}

int AI::getDepth() const {
	return this->_depth;
}

static bool compareMoves(const Move &a, const Move &b) {
	return a.score > b.score;
}

//returns a score depending on what the line contains
int AI::_evaluateLine(int count, int openEnds, bool isAi) const {
	int score = 0;
	if (count >= 5) {
		score = 100000;       //5 stones: winning line
	} else if (count == 4) {
		if (openEnds == 2)
			score = 10000;    //4 stones, 2 open ends: near-certain win
		else if (openEnds == 1)
			score = 1000;     //4 stones, 1 open end: strong threat
	} else if (count == 3) {
		if (openEnds == 2)
			score = 500;      //3 stones, 2 open ends: open three
		else if (openEnds == 1)
			score = 100;      //3 stones, 1 open end
	} else if (count == 2) {
		if (openEnds == 2)
			score = 50;       //2 stones, 2 open ends
		else if (openEnds == 1)
			score = 10;       //2 stones, 1 open end
	}
	return isAi ? score : -score; // negative score for opponent alignments
}

// Heuristic: scores the full board state from the AI's perspective.
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
					if (board.getStone(prevX, prevY) == currentStone)
						continue;
				}

				int count = 1;
				int openEnds = 0;

				//looking backward to see if it's empty
				if (prevX >= 0 && prevX < 19 && prevY >= 0 && prevY < 19) {
					if (board.getStone(prevX, prevY) == EMPTY)
						openEnds++;
				}

				//count consecutive same-color stones forward
				int nx = x + dx;
				int ny = y + dy;
				while (nx >= 0 && nx < 19 && ny >= 0 && ny < 19
					&& board.getStone(nx, ny) == currentStone) {
					count++;
					nx += dx;
					ny += dy;
				}

				//searching open end forward
				if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19) {
					if (board.getStone(nx, ny) == EMPTY)
						openEnds++;
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
				continue; //skip the cell itself

			int nx = x + dx;
			int ny = y + dy;

			if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19) {
				if (board.getStone(nx, ny) != EMPTY)
					return true; //found a non-empty neighbor
			}
		}
	}
	return false; //no neighbor found in the entire square
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
					m.score = this->_evaluateMoveScore(board, x, y);
					moves.push_back(m);
				}
			}
		}
	}

	//if the board is still empty, play the center
	if (isEmptyBoard) {
		Move m;
		m.x     = 9;
		m.y     = 9;
		m.score = 0;
		moves.push_back(m);
	}

	//sort best-to-worst so alpha-beta pruning is most effective.
	std::sort(moves.begin(), moves.end(), compareMoves);

	//beam search: keep only the top 15 candidates.
	if (moves.size() > 15)
		moves.resize(15);

	return moves;
}

//minimax with alpha-beta pruning
//with an immediate terminal check after each simulated move so
//that winning/losing positions are detected without wasting further
//recursion (avoids exploring sub-trees below a decided position)
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

			//terminal check: AI just won, no need to go deeper
			bool winByCapture = (nextBoard.getCaptures(this->_aiTeam) >= 5);
			if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_aiTeam) == WIN
				|| winByCapture)
				return 1000000 + depth; //depth bonus rewards shorter wins

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

			//terminal check: opponent just won — no need to go deeper
			bool winByCapture = (nextBoard.getCaptures(this->_opponentTeam) >= 5);
			if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_opponentTeam) == WIN
				|| winByCapture)
				return -(1000000 + depth); //depth bonus penalises longer losses

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

//returns coordinates of the best move for the AI on the current board
//and the opening-book scan breaks out as soon as stoneCount
//exceeds 1, avoiding a full 361-cell scan on every subsequent turn
Move AI::getBestMove(const Board &board) {
	//opening book: react to the very first human stone
	int stoneCount = 0;
	int hx = -1, hy = -1;
	for (int y = 0; y < 19 && stoneCount <= 1; ++y) {
		for (int x = 0; x < 19 && stoneCount <= 1; ++x) {
			if (board.getStone(x, y) != EMPTY) {
				stoneCount++;
				hx = x;
				hy = y;
			}
		}
	}
	//if exactly one stone is on the board, play adjacent toward center
	if (stoneCount == 1) {
		Move m;
		m.x     = (hx <= 9) ? hx + 1 : hx - 1;
		m.y     = (hy <= 9) ? hy + 1 : hy - 1;
		m.score = 0;
		return m;
	}


	std::vector<Move> moves = this->_generateMoves(board);

	if (moves.empty()) {
		Move bestMove;
		bestMove.x     = -1;
		bestMove.y     = -1; // board is full: draw
		bestMove.score = 0;
		return bestMove;
	}

	if (moves.size() == 1)
		return moves[0]; //only one candidate: return it immediately

	Move bestMove;
	bestMove.x     = -1;
	bestMove.y     = -1;
	bestMove.score = 0;

	int bestScore = -2000000;
	int alpha     = -2000000;
	int beta      =  2000000;

	for (size_t i = 0; i < moves.size(); ++i) {
		Board nextBoard = board;
		nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);
		nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);

		//immediate win: no need to search further.
		bool winByCapture = (nextBoard.getCaptures(this->_aiTeam) >= 5);
		if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_aiTeam) == WIN
			|| winByCapture)
			return moves[i];

		int score = this->_minimax(nextBoard, this->_depth - 1, alpha, beta, false);

		if (score > bestScore) {
			bestScore = score;
			bestMove = moves[i]; //keeping the coordinates of the new move that got more pts
		}
		if (score > alpha)
			alpha = score;
	}

	return bestMove;
}

//getting a move score for ranking moves before passing them to minimax, for a faster AI player
int AI::_evaluateMoveScore(const Board &board, int x, int y) const {
	int score = 0;

	//immediate win by alignment
	if (board.checkWin(x, y, this->_aiTeam) == WIN)
		return 10000000;

	//immediate win by 5th capture
	if (board.willCapture(x, y, this->_aiTeam)
		&& board.getCaptures(this->_aiTeam) >= 4)
		return 10000000;

	//must block opponent's winning alignment
	if (board.checkWin(x, y, this->_opponentTeam) == WIN)
		score += 5000000; //urgent

	//must block opponent's 5th capture
	if (board.willCapture(x, y, this->_opponentTeam)
		&& board.getCaptures(this->_opponentTeam) >= 4)
		score += 5000000;

	//captures and blocks
	if (board.willCapture(x, y, this->_aiTeam))
		score += 10000;
	if (board.willCapture(x, y, this->_opponentTeam))
		score += 9000;

	//scan 4 axes to score alignment potential
	int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

	for (int d = 0; d < 4; ++d) {
		int dx = directions[d][0];
		int dy = directions[d][1];

		int aiStones = 0;
		int oppStones = 0;

		//both directions of an axe
		for (int dir = -1; dir <= 1; dir += 2) {
			bool gapUsed = false; // allow at most one empty gap per half
			for (int step = 1; step <= 4; ++step) {
				int nx = x + (dx * dir * step);
				int ny = y + (dy * dir * step);

				if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19)
					break; // out of board

				e_stone s = board.getStone(nx, ny);
				if (s == this->_aiTeam) {
					aiStones++;
					score += (100 / step); // closer stones score higher
				} else if (s == this->_opponentTeam) {
					oppStones++;
					score += (80 / step); //getting less by blocking than attacking
				} else {
					//empty cell: allow one gap to detect X.XX patterns
					if (!gapUsed) {
						gapUsed = true;
						continue; // look through the gap
					}
					break; // second gap — stop scanning this half
				}
			}
		}

		//bonus for building or blocking long alignments
		if (aiStones >= 3)      score += 5000; // forms a 4-in-a-row
		else if (aiStones == 2) score += 1000; // forms a 3-in-a-row

		if (oppStones >= 3)      score += 4500; // blocks a 4-in-a-row
		else if (oppStones == 2) score += 800;  // blocks a 3-in-a-row
	}

	return score;
}