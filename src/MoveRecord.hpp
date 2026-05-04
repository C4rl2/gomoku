#ifndef MOVE_RECORD_HPP
# define MOVE_RECORD_HPP

# include "Board.hpp"

//timeline snapshot used by Game to support undo/redo/history navigation
//holds a full copy of the restorable game state after a move is applied
class MoveRecord {
	public:
		Board		_board;
		e_stone		_currentPlayer;
		bool		_gameOver;
		int			_winner;
		int			_moveX;
		int			_moveY;
		e_stone		_playedStone;

		MoveRecord();
		MoveRecord(const Board &board, e_stone currentPlayer, bool gameOver, int winner,
			int moveX, int moveY, e_stone playedStone);
		MoveRecord(const MoveRecord &other);
		MoveRecord &operator=(const MoveRecord &other);
		~MoveRecord();
};

#endif
