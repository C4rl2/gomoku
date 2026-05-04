#include "MoveRecord.hpp"

MoveRecord::MoveRecord()
	: _board(), _currentPlayer(BLACK), _gameOver(false), _winner(0),
	_moveX(-1), _moveY(-1), _playedStone(EMPTY) {}

MoveRecord::MoveRecord(const Board &board, e_stone currentPlayer, bool gameOver, int winner,
	int moveX, int moveY, e_stone playedStone)
	: _board(board), _currentPlayer(currentPlayer), _gameOver(gameOver), _winner(winner),
	_moveX(moveX), _moveY(moveY), _playedStone(playedStone) {}

MoveRecord::MoveRecord(const MoveRecord &other) {
	*this = other;
}

MoveRecord &MoveRecord::operator=(const MoveRecord &other) {
	if (this != &other) {
		this->_board         = other._board;
		this->_currentPlayer = other._currentPlayer;
		this->_gameOver      = other._gameOver;
		this->_winner        = other._winner;
		this->_moveX         = other._moveX;
		this->_moveY         = other._moveY;
		this->_playedStone   = other._playedStone;
	}
	return *this;
}

MoveRecord::~MoveRecord() {}
