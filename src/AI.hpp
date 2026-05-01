#ifndef AI_HPP
# define AI_HPP

# include "Board.hpp"
# include <limits>
# include <vector>
# include <algorithm>

struct Move {
	int x;
	int y;
	int score;
};

//transposition table entry flags
enum e_tt_flag {
	TT_EXACT = 0, //score is exact
	TT_LOWER = 1, //score is a lower bound (alpha cut)
	TT_UPPER = 2  //score is an upper bound (beta cut)
};

//one slot in the transposition table
struct TTEntry {
	unsigned long long	hash;  //full 64-bit hash to detect collisions
	int					score; //stored score for this position
	int					depth; //depth at which this score was computed
	e_tt_flag			flag;  //exact / lower / upper bound
};

# define TT_SIZE     1048576 //1M entries (must be a power of 2)
# define TT_MASK     (TT_SIZE - 1)
# define ZOBRIST_AI  0 //index for ai stone in zobrist table
# define ZOBRIST_OPP 1 //index for opponent stone in zobrist table

class AI {
	private:
		e_stone		_aiTeam;
		e_stone		_opponentTeam;
		int			_depth;

		//zobrist random table: [y][x][player_index]
		unsigned long long	_zobristTable[19][19][2];
		//transposition table (heap-allocated, TT_SIZE entries)
		TTEntry				*_ttable;

		int					_evaluateBoard(const Board &board) const;
		int					_evaluateLine(int count, int openEnds, bool isAi) const;
		bool				_hasNeighbor(const Board &board, int x, int y, int distance) const;
		std::vector<Move>	_generateMoves(const Board &board) const;
		int					_minimax(Board board, int depth, int alpha, int beta,
									bool isMaximizing, unsigned long long hash);
		int					_evaluateMoveScore(const Board &board, int x, int y) const;
		int					_countThreats(const Board &board, int x, int y, e_stone stone) const;
		unsigned long long	_computeHash(const Board &board) const;
		void				_initZobrist();
		void				_clearTT();

	public:
		AI();
		AI(e_stone aiTeam);
		AI(const AI &other);
		AI &operator=(const AI &other);
		~AI();

		void	setDepth(int depth);
		int		getDepth() const;
		Move	getBestMove(const Board &board);
};

#endif