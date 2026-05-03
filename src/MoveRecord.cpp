#include "MoveRecord.hpp"

MoveRecord::MoveRecord() : _board(), _currentPlayer(BLACK), _gameOver(false), _winner(0) {}

MoveRecord::MoveRecord(const Board &board, e_stone currentPlayer, bool gameOver, int winner)
	: _board(board), _currentPlayer(currentPlayer), _gameOver(gameOver), _winner(winner) {}

MoveRecord::MoveRecord(const MoveRecord &other) {
	*this = other;
}

MoveRecord &MoveRecord::operator=(const MoveRecord &other) {
	if (this != &other) {
		this->_board         = other._board;
		this->_currentPlayer = other._currentPlayer;
		this->_gameOver      = other._gameOver;
		this->_winner        = other._winner;
	}
	return *this;
}

MoveRecord::~MoveRecord() {}
