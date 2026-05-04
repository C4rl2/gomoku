#ifndef AI_HPP
# define AI_HPP

# include "Board.hpp"
# include <limits>
# include <vector>
# include <algorithm>

# ifdef __EMSCRIPTEN__
#  include <emscripten/emscripten.h>
# endif

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

# define TT_SIZE        1048576 //1M entries (must be a power of 2)
# define TT_MASK        (TT_SIZE - 1)
# define ZOBRIST_AI     0 //index for ai stone in zobrist table
# define ZOBRIST_OPP    1 //index for opponent stone in zobrist table
# define TIME_BUDGET_MS 496 //iterative deepening time budget in milliseconds

class AI {
	private:
		e_stone		_aiTeam;
		e_stone		_opponentTeam;
		int			_depth;

		//zobrist random table: [y][x][player_index]
		unsigned long long	_zobristTable[19][19][2];
		//transposition table (heap-allocated, TT_SIZE entries)
		TTEntry				*_ttable;

		//iterative deepening time tracking
		double				_startTime;  //time when getBestMove was called (ms)
		int					_lastDepth;  //depth actually reached in the last getBestMove call
		bool				_timeUp() const; //returns true if TIME_BUDGET_MS elapsed

		//per-move instrumentation counters reset at the start of getBestMove
		int		_statNodes;            //nb of _minimax invocations executed
		int		_statCutoffs;          //nb of alpha-beta beta<=alpha breaks
		int		_statTTHits;           //nb of transposition table reuses
		double	_statTimeHeuristic;    //ms accumulated inside _evaluateBoard
		double	_statTimeMoveOrdering; //ms accumulated inside _generateMoves
		double	_statTimeZobrist;      //ms accumulated inside _computeHash
		double	_statTimeTT;           //ms accumulated inside TT probe and store
		double	_statTimeTotal;        //ms total wall-clock spent in getBestMove
		void	_resetStats();         //zero all counters above

		int					_evaluateBoard(const Board &board) const;
		int					_evaluateLine(int count, int openEnds, bool isAi) const;
		//returns count + adjacent empty cells in both directions of the axis;
		//if the result is < 5, the alignment can never extend to a 5-in-a-row
		int					_developableSpace(const Board &board, int x, int y,
											int dx, int dy, int count) const;
		//returns the number of vulnerable pairs that placing `stone` at (x, y)
		//would create on `board`; used by move ordering to discourage moves
		//that hand the opponent a free capture next ply
		int					_createsVulnerablePair(const Board &board, int x, int y,
											e_stone stone) const;
		bool				_hasNeighbor(const Board &board, int x, int y, int distance) const;
		std::vector<Move>	_generateMoves(const Board &board) const;
		int					_minimax(Board board, int depth, int alpha, int beta,
									bool isMaximizing, unsigned long long hash);
		int					_evaluateMoveScore(const Board &board, int x, int y) const;
		int					_countThreats(const Board &board, int x, int y, e_stone stone) const;
		unsigned long long	_computeHash(const Board &board) const;
		void				_initZobrist();
		void				_clearTT();
		//returns current time in milliseconds (WASM or native)
		static double		_now();

	public:
		AI();
		AI(e_stone aiTeam);
		AI(const AI &other);
		AI &operator=(const AI &other);
		~AI();

		void	setDepth(int depth);
		int		getDepth() const;
		int		getLastDepth() const;
		//rebinds which team the ai maximises for; clears the tt when the team
		//actually changes because zobrist indices are relative to _aiTeam
		void	setAiTeam(e_stone team);
		Move	getBestMove(const Board &board);

		//instrumentation getters: values reflect the last getBestMove call
		int		getLastNodes() const;
		int		getLastCutoffs() const;
		int		getLastTTHits() const;
		double	getLastTimeHeuristic() const;
		double	getLastTimeMoveOrdering() const;
		double	getLastTimeZobrist() const;
		double	getLastTimeTT() const;
		double	getLastTimeTotal() const;
};

#endif