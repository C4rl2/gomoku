#ifndef AI_HPP
# define AI_HPP

# include "Board.hpp"
# include <limits> //alpha-beta
# include <vector>

struct Move {
	int x;
	int y;
};

class AI {
	private:
		e_stone _aiTeam;
		e_stone _opponentTeam;

		int     _evaluateBoard(const Board &board) const;
		int     _evaluateLine(int count, int openEnds, bool isAi) const;
		bool	_hasNeighbor(const Board &board, int x, int y, int distance) const;
		std::vector<Move>	_generateMoves(const Board &board) const;

	public:
		AI();
		AI(e_stone aiTeam);
		AI(const AI &other);
		AI &operator=(const AI &other);
		~AI();

		Move	getBestMove(const Board &board);
};

#endif