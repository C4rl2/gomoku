#include "Board.hpp"

Board::Board() {
	for (int y = 0; y < 19; ++y) {
		for (int x = 0; x < 19; ++x) {
			this->_grid[y][x] = EMPTY;
		}
	}
}

Board::Board(const Board &other) {
	*this = other;
}

Board &Board::operator=(const Board &other) {
	if (this != &other) {
		for (int y = 0; y < 19; ++y) {
			for (int x = 0; x < 19; ++x) {
				this ->_grid[y][x] = other._grid[y][x];
			}
		}
	}
	return *this;
}

Board::~Board() {}

e_stone Board::getStone(int x, int y) const {
	if (x >= 0 && x < 19 && y >= 0 && y < 19) {
		return this->_grid[y][x];
	}
	return EMPTY; //if outside the board
}

//place a stone (return false if occupied or outside the board)
bool Board::setStone(int x, int y, e_stone stone) {
	if (x >= 0 && x < 19 && y >= 0 && y < 19) {
		if (this->_grid[y][x] == EMPTY) {
			this->_grid[y][x] = stone;
			return true;
		}
	}
	return false; //invalid placement
}

void Board::printBoard() const {
	std::cout << " 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8" << std::endl;
	for (int y = 0; y < 19; ++y) {
		if (y < 10)
			std::cout << y << " ";
		else
			std::cout << y;
		for (int x = 0; x < 19; ++x) {
			if (this->_grid[y][x] == EMPTY)
				std::cout << ". ";
			else if (this->_grid[y][x] == BLACK)
				std::cout << "X "; //X for black
			else
				std::cout << "O "; //O for white
		}
		std::cout << std::endl;
	}
}