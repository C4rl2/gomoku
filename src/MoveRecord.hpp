#ifndef MOVE_RECORD_HPP
# define MOVE_RECORD_HPP

# include "Board.hpp"

//pre-move snapshot used by Game to support undoMove
//holds a full copy of the restorable game state before a move is applied
class MoveRecord {
	public:
		Board		_board;
		e_stone		_currentPlayer;
		bool		_gameOver;
		int			_winner;

		MoveRecord();
		MoveRecord(const Board &board, e_stone currentPlayer, bool gameOver, int winner);
		MoveRecord(const MoveRecord &other);
		MoveRecord &operator=(const MoveRecord &other);
		~MoveRecord();
};

#endif
