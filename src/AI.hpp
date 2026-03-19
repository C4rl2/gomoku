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

class AI {
	private:
		e_stone _aiTeam;
		e_stone _opponentTeam;

		int     _evaluateBoard(const Board &board) const;
		int     _evaluateLine(int count, int openEnds, bool isAi) const;
		bool	_hasNeighbor(const Board &board, int x, int y, int distance) const;
		std::vector<Move>	_generateMoves(const Board &board) const;
		int		_minimax(Board board, int depth, int alpha, int beta, bool isMaximizing);
		int		_evaluateMoveScore(const Board &board, int x, int y) const;

	public:
		AI();
		AI(e_stone aiTeam);
		AI(const AI &other);
		AI &operator=(const AI &other);
		~AI();

		Move	getBestMove(const Board &board);
};

#endif