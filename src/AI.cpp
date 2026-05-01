#include "AI.hpp"

#ifdef __EMSCRIPTEN__
# include <emscripten/emscripten.h>
#else
# include <ctime>
#endif

static bool compareMoves(const Move &a, const Move &b) {
	return a.score > b.score ;
}

//lcg pseudo-random generator seeded manually (no rand() to stay deterministic)
//produces a 64-bit value from a mutable state
static unsigned long long lcg64(unsigned long long &state) {
	state = state * 6364136223846793005ULL + 1442695040888963407ULL;
	return state;
}

//returns current wall-clock time in milliseconds
//uses emscripten_get_now in WASM (accurate), clock() as fallback on native
double AI::_now() {
#ifdef __EMSCRIPTEN__
	return emscripten_get_now();
#else
	return (double)clock() / (CLOCKS_PER_SEC / 1000.0);
#endif
}

//returns true if the time budget has been exhausted
bool AI::_timeUp() const {
	return (AI::_now() - this->_startTime) >= (double)TIME_BUDGET_MS;
}

//initialise zobrist table and allocate the transposition table
AI::AI() : _aiTeam(WHITE), _opponentTeam(BLACK), _depth(5), _startTime(0.0), _lastDepth(0) {
	_ttable = new TTEntry[TT_SIZE];
	_initZobrist();
	_clearTT();
}

AI::AI(e_stone aiTeam) : _aiTeam(aiTeam), _depth(5), _startTime(0.0), _lastDepth(0) {
	this->_opponentTeam = (aiTeam == BLACK) ? WHITE : BLACK;
	_ttable = new TTEntry[TT_SIZE];
	_initZobrist();
	_clearTT();
}

AI::AI(const AI &other) : _ttable(NULL), _startTime(0.0), _lastDepth(0) {
	*this = other;
}

AI &AI::operator=(const AI &other) {
	if (this != &other) {
		this->_aiTeam       = other._aiTeam;
		this->_opponentTeam = other._opponentTeam;
		this->_depth        = other._depth;
		this->_startTime    = other._startTime;
		this->_lastDepth    = other._lastDepth;
		//copy zobrist table
		for (int y = 0; y < 19; ++y)
			for (int x = 0; x < 19; ++x)
				for (int p = 0; p < 2; ++p)
					this->_zobristTable[y][x][p] = other._zobristTable[y][x][p];
		//reallocate and copy transposition table
		if (!this->_ttable)
			this->_ttable = new TTEntry[TT_SIZE];
		for (int i = 0; i < TT_SIZE; ++i)
			this->_ttable[i] = other._ttable[i];
	}
	return *this;
}

AI::~AI() {
	delete[] _ttable;
}

void AI::setDepth(int depth) {
	this->_depth = depth;
}

int AI::getDepth() const {
	return this->_depth;
}

int AI::getLastDepth() const {
	return this->_lastDepth;
}

//fills _zobristTable with deterministic pseudo-random 64-bit values
void AI::_initZobrist() {
	unsigned long long state = 0xDEADBEEFCAFEBABEULL; //fixed seed for determinism
	for (int y = 0; y < 19; ++y)
		for (int x = 0; x < 19; ++x)
			for (int p = 0; p < 2; ++p)
				_zobristTable[y][x][p] = lcg64(state);
}

//resets all transposition table entries to an invalid state
void AI::_clearTT() {
	for (int i = 0; i < TT_SIZE; ++i) {
		_ttable[i].hash  = 0;
		_ttable[i].score = 0;
		_ttable[i].depth = -1; //depth -1 marks the slot as empty
		_ttable[i].flag  = TT_EXACT;
	}
}

//computes the full zobrist hash of a board from scratch
//used once at the root; incremental updates happen inside _minimax
unsigned long long AI::_computeHash(const Board &board) const {
	unsigned long long h = 0;
	for (int y = 0; y < 19; ++y) {
		for (int x = 0; x < 19; ++x) {
			e_stone s = board.getStone(x, y);
			if (s == this->_aiTeam)
				h ^= _zobristTable[y][x][ZOBRIST_AI];
			else if (s == this->_opponentTeam)
				h ^= _zobristTable[y][x][ZOBRIST_OPP];
		}
	}
	return h;
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
bool AI::_hasNeighbor(const Board &board, int x, int y, int distance) const {
	for (int dy = -distance; dy <= distance; ++dy) {
		for (int dx = -distance; dx <= distance; ++dx) {
			if (dx == 0 && dy == 0)
				continue; //stone itself

			int nx = x + dx;
			int ny = y + dy;

			if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19) {
				if (board.getStone(nx, ny) != EMPTY)
					return true; //found a neighbor
			}
		}
	}
	return false; //cell has absolutely no neighbor stones all around
}

//scan the board and return list of moves (within all stones neighbors)
std::vector<Move> AI::_generateMoves(const Board &board) const {
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
	
	//if AI is playing the first move on the board
	if (isEmptyBoard) {
		Move m;
		m.x = 9;
		m.y = 9;
		m.score = 0;
		moves.push_back(m);
	}

	std::sort(moves.begin(), moves.end(), compareMoves); //best to worse move in the list for minimax to get faster

	if (moves.size() > 15)
		moves.resize(15); //beam search, only keeping best moves possible

	return moves;
}

//minimax algo with alpha-beta pruning and zobrist transposition table
//hash is passed by value and updated incrementally at each recursive level
//terminal check added after each move so won positions skip useless recursion
//returns INT_MIN (sentinel) when time budget is exceeded mid-search
int AI::_minimax(Board board, int depth, int alpha, int beta, bool isMaximizing, unsigned long long hash) {
	//abort if time budget exceeded — sentinel propagates up to getBestMove
	if (this->_timeUp())
		return (isMaximizing ? -2000001 : 2000001);

	//probe transposition table before doing any work
	unsigned int ttIndex = (unsigned int)(hash & TT_MASK);
	TTEntry &entry = _ttable[ttIndex];

	if (entry.depth >= depth && entry.hash == hash) {
		//slot matches this position and was computed at least as deep
		if (entry.flag == TT_EXACT)
			return entry.score; //exact score: use it directly
		if (entry.flag == TT_LOWER && entry.score > alpha)
			alpha = entry.score; //tighten alpha with stored lower bound
		if (entry.flag == TT_UPPER && entry.score < beta)
			beta = entry.score; //tighten beta with stored upper bound
		if (alpha >= beta)
			return entry.score; //window already closed: prune
	}

	//if we reach max depth we stop recursing
	if (depth == 0) {
		int score = this->_evaluateBoard(board);
		//store exact leaf score in the table
		entry.hash  = hash;
		entry.score = score;
		entry.depth = 0;
		entry.flag  = TT_EXACT;
		return score;
	}

	//list of possible moves
	std::vector<Move> moves = this->_generateMoves(board);

	if (moves.empty()) {
		int score = this->_evaluateBoard(board);
		entry.hash  = hash;
		entry.score = score;
		entry.depth = depth;
		entry.flag  = TT_EXACT;
		return score;
	}

	//remember original alpha/beta to determine the flag after the loop
	int origAlpha = alpha;
	int origBeta  = beta;
	int bestScore = isMaximizing ? -2000000 : 2000000;

	if (isMaximizing) {
		//AI's turn, searching for highest value (alpha)
		for (size_t i = 0; i < moves.size(); ++i) {
			Board nextBoard = board; //copying the board
			//playing the simulated move
			nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);

			//compute hash delta for placed stone before captures
			unsigned long long nextHash = hash ^ _zobristTable[moves[i].y][moves[i].x][ZOBRIST_AI];

			//execute captures and update hash for each removed stone
			int captureDirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
			for (int d = 0; d < 8; ++d) {
				int dx = captureDirs[d][0];
				int dy = captureDirs[d][1];
				int nx3 = moves[i].x + 3 * dx;
				int ny3 = moves[i].y + 3 * dy;
				if (nx3 >= 0 && nx3 < 19 && ny3 >= 0 && ny3 < 19) {
					int ox1 = moves[i].x + dx,  oy1 = moves[i].y + dy;
					int ox2 = moves[i].x + 2*dx, oy2 = moves[i].y + 2*dy;
					if (board.getStone(ox1, oy1) == this->_opponentTeam &&
						board.getStone(ox2, oy2) == this->_opponentTeam &&
						board.getStone(nx3, ny3) == this->_aiTeam) {
						//these two opponent stones will be captured: remove from hash
						nextHash ^= _zobristTable[oy1][ox1][ZOBRIST_OPP];
						nextHash ^= _zobristTable[oy2][ox2][ZOBRIST_OPP];
					}
				}
			}
			nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);

			//if AI just won, no need to go deeper
			bool winByCapture = (nextBoard.getCaptures(this->_aiTeam) >= 5);
			if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_aiTeam) == WIN
				|| winByCapture)
				return 1000000 + depth; //depth bonus rewards shorter wins

			//recursive, getting deeper, with the opponent (humain) move
			int eval = this->_minimax(nextBoard, depth - 1, alpha, beta, false, nextHash);

			//propagate timeout sentinel without storing to TT
			if (eval == -2000001 || eval == 2000001)
				return eval;

			if (eval > bestScore)
				bestScore = eval;
			if (eval > alpha)
				alpha = eval; //this branch got the higher nb of pts

			if (beta <= alpha)
				break; //if humain has a better move, pruning
		}
	} else {
		//humain's turn, searching for lowest value (beta)
		for (size_t i = 0; i < moves.size(); ++i) {
			Board nextBoard = board; //copying the board
			//playing the simulated move
			nextBoard.setStone(moves[i].x, moves[i].y, this->_opponentTeam);

			//compute hash delta for placed stone before captures
			unsigned long long nextHash = hash ^ _zobristTable[moves[i].y][moves[i].x][ZOBRIST_OPP];

			//execute captures and update hash for each removed stone
			int captureDirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
			for (int d = 0; d < 8; ++d) {
				int dx = captureDirs[d][0];
				int dy = captureDirs[d][1];
				int nx3 = moves[i].x + 3 * dx;
				int ny3 = moves[i].y + 3 * dy;
				if (nx3 >= 0 && nx3 < 19 && ny3 >= 0 && ny3 < 19) {
					int ox1 = moves[i].x + dx,  oy1 = moves[i].y + dy;
					int ox2 = moves[i].x + 2*dx, oy2 = moves[i].y + 2*dy;
					if (board.getStone(ox1, oy1) == this->_aiTeam &&
						board.getStone(ox2, oy2) == this->_aiTeam &&
						board.getStone(nx3, ny3) == this->_opponentTeam) {
						//these two ai stones will be captured: remove from hash
						nextHash ^= _zobristTable[oy1][ox1][ZOBRIST_AI];
						nextHash ^= _zobristTable[oy2][ox2][ZOBRIST_AI];
					}
				}
			}
			nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_opponentTeam);

			//if opponent just won, no need to go deeper
			bool winByCapture = (nextBoard.getCaptures(this->_opponentTeam) >= 5);
			if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_opponentTeam) == WIN
				|| winByCapture)
				return -(1000000 + depth); //depth bonus penalises longer losses

			//recursive, getting deeper, with the opponent (AI) move
			int eval = this->_minimax(nextBoard, depth - 1, alpha, beta, true, nextHash);

			//propagate timeout sentinel without storing to TT
			if (eval == -2000001 || eval == 2000001)
				return eval;

			if (eval < bestScore)
				bestScore = eval;
			if (eval < beta)
				beta = eval; //this branch got the lowest nb of pts

			if (beta <= alpha)
				break; //if AI has better move elsewhere, won't go with this branch
		}
	}

	//store result in transposition table with correct flag
	e_tt_flag flag;
	if (bestScore <= origAlpha)
		flag = TT_UPPER; //we never raised alpha: upper bound
	else if (bestScore >= origBeta)
		flag = TT_LOWER; //we caused a cutoff: lower bound
	else
		flag = TT_EXACT; //score is within the window: exact

	//only overwrite if new entry is deeper (depth-preferred replacement)
	if (depth >= entry.depth || entry.hash != hash) {
		entry.hash  = hash;
		entry.score = bestScore;
		entry.depth = depth;
		entry.flag  = flag;
	}

	return bestScore;
}

//return coordinates of the best possible move
//uses iterative deepening from depth 1 up to _depth within TIME_BUDGET_MS
//the TT is preserved across iterations (depth-preferred replacement handles it)
//the TT is cleared once per real move, not between ID iterations
Move AI::getBestMove(const Board &board) {
	//clear TT at the start of each real move (not between ID iterations)
	_clearTT();
	this->_lastDepth = 0;

	//record start time for the budget shared across all ID iterations
	this->_startTime = AI::_now();

	//hardcoding opening book
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
	//if humain played the first move of the game
	if (stoneCount == 1) {
		Move m;
		//playing diag towards center, or towards the outside if already center
		m.x = (hx <= 9) ? hx + 1 : hx - 1;
		m.y = (hy <= 9) ? hy + 1 : hy - 1;
		m.score = 0;
		return m;
	}

	std::vector<Move> moves = this->_generateMoves(board);
	
	if (moves.empty()) {
		Move bestMove;
		bestMove.x = -1;
		bestMove.y = -1; //if board is full
		bestMove.score = 0;
		return bestMove;
	}

	if (moves.size() == 1) {
		return moves[0]; //if empty board, play middle without losing time
	}

	//compute the root hash once; children update it incrementally
	unsigned long long rootHash = _computeHash(board);

	//check for immediate wins at root before launching ID
	for (size_t i = 0; i < moves.size(); ++i) {
		Board nextBoard = board;
		nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);
		nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);
		bool winByCapture = (nextBoard.getCaptures(this->_aiTeam) >= 5);
		if (nextBoard.checkWin(moves[i].x, moves[i].y, this->_aiTeam) == WIN
			|| winByCapture)
			return moves[i];
	}

	//iterative deepening: depth 1 to _depth, stop if time runs out
	//bestMove is always updated at the end of a fully completed iteration
	Move bestMove = moves[0]; //fallback: best move from beam ordering (depth 0)

	for (int currentDepth = 1; currentDepth <= this->_depth; ++currentDepth) {
		//abort this iteration if budget already gone before we start
		if (this->_timeUp())
			break;

		Move  iterBest      = moves[0];
		int   iterBestScore = -2000000;
		int   alpha         = -2000000;
		int   beta          =  2000000;
		bool  aborted       = false;

		for (size_t i = 0; i < moves.size(); ++i) {
			if (this->_timeUp()) {
				aborted = true;
				break;
			}

			Board nextBoard = board;
			nextBoard.setStone(moves[i].x, moves[i].y, this->_aiTeam);

			//compute hash delta for placed stone before captures
			unsigned long long childHash = rootHash ^ _zobristTable[moves[i].y][moves[i].x][ZOBRIST_AI];

			//update hash for captures that will occur
			int captureDirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
			for (int d = 0; d < 8; ++d) {
				int dx = captureDirs[d][0];
				int dy = captureDirs[d][1];
				int nx3 = moves[i].x + 3 * dx;
				int ny3 = moves[i].y + 3 * dy;
				if (nx3 >= 0 && nx3 < 19 && ny3 >= 0 && ny3 < 19) {
					int ox1 = moves[i].x + dx,  oy1 = moves[i].y + dy;
					int ox2 = moves[i].x + 2*dx, oy2 = moves[i].y + 2*dy;
					if (board.getStone(ox1, oy1) == this->_opponentTeam &&
						board.getStone(ox2, oy2) == this->_opponentTeam &&
						board.getStone(nx3, ny3) == this->_aiTeam) {
						childHash ^= _zobristTable[oy1][ox1][ZOBRIST_OPP];
						childHash ^= _zobristTable[oy2][ox2][ZOBRIST_OPP];
					}
				}
			}
			nextBoard.executeCaptures(moves[i].x, moves[i].y, this->_aiTeam);

			//launching minimax with next move being the humain one
			int score = this->_minimax(nextBoard, currentDepth - 1, alpha, beta, false, childHash);

			//sentinel from _minimax means time ran out mid-search: discard partial result
			if (score == -2000001 || score == 2000001) {
				aborted = true;
				break;
			}

			if (score > iterBestScore) {
				iterBestScore = score;
				iterBest      = moves[i];
			}
			if (score > alpha)
				alpha = score;
		}

		//only commit this iteration's result if it completed without abort
		if (!aborted) {
			bestMove       = iterBest;
			this->_lastDepth = currentDepth;
			//re-order moves so next iteration starts from the best candidate
			//simple: bring iterBest to front if it moved in moves[]
			for (size_t i = 1; i < moves.size(); ++i) {
				if (moves[i].x == iterBest.x && moves[i].y == iterBest.y) {
					Move tmp = moves[0];
					moves[0] = moves[i];
					moves[i] = tmp;
					break;
				}
			}
		}
	}

	return bestMove;
}

//counts how many distinct axes carry a strong threat for stone after
//placing it at (x, y) — the stone must already be set on board before calling
//a threat on an axis = 3 stones with >=1 open end, or 4+ stones
//returns the number of threatening axes (0 to 4)
int AI::_countThreats(const Board &board, int x, int y, e_stone stone) const {
	int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
	int threatCount = 0;

	for (int d = 0; d < 4; ++d) {
		int dx = directions[d][0];
		int dy = directions[d][1];

		int count    = 1; //the stone just placed counts as 1
		int openEnds = 0;

		//scan forward along the axis
		int nx = x + dx;
		int ny = y + dy;
		while (nx >= 0 && nx < 19 && ny >= 0 && ny < 19
			&& board.getStone(nx, ny) == stone) {
			count++;
			nx += dx;
			ny += dy;
		}
		//check if the forward end is open
		if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19
			&& board.getStone(nx, ny) == EMPTY)
			openEnds++;

		//scan backward along the axis
		int px = x - dx;
		int py = y - dy;
		while (px >= 0 && px < 19 && py >= 0 && py < 19
			&& board.getStone(px, py) == stone) {
			count++;
			px -= dx;
			py -= dy;
		}
		//check if the backward end is open
		if (px >= 0 && px < 19 && py >= 0 && py < 19
			&& board.getStone(px, py) == EMPTY)
			openEnds++;

		//axis is a threat if 4+ stones, or 3 stones with at least one open end
		if (count >= 4 || (count == 3 && openEnds >= 1))
			threatCount++;
	}
	return threatCount;
}

//getting a move score for ranking moves before passing them to minimax, for a faster AI player
int AI::_evaluateMoveScore(const Board &board, int x, int y) const {
	int score = 0;

	//winning the game by alignement
	if (board.checkWin(x, y, this->_aiTeam) == WIN)
		return 10000000; 

	//5th capture (winning too)
	if (board.willCapture(x, y, this->_aiTeam) && board.getCaptures(this->_aiTeam) >= 4)
		return 10000000;

	//if opponenent was getting a 5th alignement (win)
	if (board.checkWin(x, y, this->_opponentTeam) == WIN)
		score += 5000000; //urgent

	//if opponenent was getting a 5th capture (win)
	if (board.willCapture(x, y, this->_opponentTeam) && board.getCaptures(this->_opponentTeam) >= 4)
		score += 5000000;

	//fork detection: simulate placing the stone then count threatening axes
	//a fork (>=2 threats) is unblockable so it gets a large bonus
	{
		Board tmpAi = board;
		tmpAi.setStone(x, y, this->_aiTeam);
		int aiThreats = this->_countThreats(tmpAi, x, y, this->_aiTeam);
		if (aiThreats >= 2)
			score += 150000; //offensive fork: two simultaneous threats
		else if (aiThreats == 1)
			score += 20000;  //single strong threat
	}
	{
		Board tmpOpp = board;
		tmpOpp.setStone(x, y, this->_opponentTeam);
		int oppThreats = this->_countThreats(tmpOpp, x, y, this->_opponentTeam);
		if (oppThreats >= 2)
			score += 120000; //defensive fork: block opponent double threat
		else if (oppThreats == 1)
			score += 15000;  //block single opponent threat
	}

	//capturing or blocking a capture
	if (board.willCapture(x, y, this->_aiTeam))
		score += 10000;
	if (board.willCapture(x, y, this->_opponentTeam))
		score += 9000;

	//alignements and threats
	//scanning 4 axes
	int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

	for (int d = 0; d < 4; ++d) {
		int dx = directions[d][0];
		int dy = directions[d][1];

		int aiStones  = 0;
		int oppStones = 0;

		//both directions of an axe
		for (int dir = -1; dir <= 1; dir += 2) {
			bool gapUsed = false; //one gap allowed per half to catch X.XX patterns
			for (int step = 1; step <= 4; ++step) {
				int nx = x + (dx * dir * step);
				int ny = y + (dy * dir * step);
                
				if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19)
					break; //getting out of the board

				e_stone s = board.getStone(nx, ny);
				if (s == this->_aiTeam) {
					aiStones++;
					score += (100 / step); //bigger bonus if closer stone
				} else if (s == this->_opponentTeam) {
					oppStones++;
					score += (80 / step); //getting less by blocking than attacking
				} else {
					if (!gapUsed) {
						gapUsed = true;
						continue; //look through the gap
					}
					break; //second gap, stop scanning this half
				}
			}
		}

		//bonus for blocking or creating big alignements
		if (aiStones >= 3)      score += 5000; //4 stones aligned
		else if (aiStones == 2) score += 1000; //3 stones aligned

		if (oppStones >= 3)      score += 4500; //blocking 4 stones alignement
		else if (oppStones == 2) score += 800;  //blocking 3 stones alignement
	}

	return score;
}