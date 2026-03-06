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

//fct that checks 4 stone in the given direction after the last placed stone
int Board::_countDirection(int x, int y, int dx, int dy, e_stone stone) const {
	int count = 0;

	//for 4 stones, getting coordinates of the wanted direction
	for (int i = 1; i <= 4; ++i) {
		int nx = x + (i * dx);
		int ny = y + (i * dy);

		//stoping if getting outside the grid
		if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19)
			break;

		//if the stone is of the color of the player, counting one more in the alignement
		if (this->_grid[ny][nx] == stone)
			count++;
		else
			break;
	}
	return count;
}

//checks if the last stone is making an alignement of 5 or more and if it is breakable
bool Board::checkWin(int x, int y, e_stone stone) const {
	//4 axes, dx and dy goes both ways
	int axes[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

	for (int i = 0; i < 4; ++i) {
		int dx = axes[i][0];
		int dy = axes[i][1];

		//counting stones aligned
		int countForward = this->_countDirection(x, y, dx, dy, stone);
		int countBackward = this->_countDirection(x, y, -dx, -dy, stone);

		//if 5 or more aligned
		if (1 + countForward + countBackward >= 5) {
			bool isBreakable = false;

			//checking if last stone played is vulnerable
			if (this->_isVulnerable(x, y, stone)) {
				isBreakable = true;
			}

			//checking if forward stones are vulnerable
			for (int j = 1; j <= countForward && !isBreakable; ++j) {
				if (this->_isVulnerable(x + (j * dx), y + (j * dy), stone)) {
					isBreakable = true;
				}
			}

			//checking if backward stones are vulnerable
			for (int j = 1; j <= countBackward && !isBreakable; ++j) {
				if (this->_isVulnerable(x - (j * dx), y - (j * dy), stone)) {
					isBreakable = true;
				}
			}

			//if no stones are vulnerable, win
			if (!isBreakable) {
				return true; 
			}
		}
	}
	return false;
}

int	Board::executeCaptures(int x, int y, e_stone stone) {
	int	capturedPairs = 0;

	e_stone	opponent = (stone == BLACK) ? WHITE : BLACK;

	//all possible directions
	int	direction[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

	for (int i = 0; i < 8; ++i) {
		int dx = direction[i][0]; //x of the eight directions
		int dy = direction[i][1]; //y of the eight directions

		//coordinates of the 3rd intersection in the given direction
		int nx3 = x + (3 * dx);
		int ny3 = y + (3 * dy);

		//memory access protection in the if
		if (nx3 >= 0 && nx3 < 19 && ny3 >= 0 && ny3 < 19) {
			//searching for XOOX or OXXO (capture)
			if (this->_grid[y + dy][x + dx] == opponent &&
				this->_grid[y + (2 * dy)][x + (2 * dx)] == opponent &&
				this->_grid[ny3][nx3] == stone) {
					//capturing
					this->_grid[y + dy][x + dx] = EMPTY;
					this->_grid[y + (2 * dy)][x + (2 * dx)] = EMPTY;
					capturedPairs++;
				}
		}
	}
	return capturedPairs;
}

bool Board::_isVulnerable(int x, int y, e_stone stone) const {
	e_stone opponent = (stone == BLACK) ? WHITE : BLACK;
	int directions[8][2] = {
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {-1, -1}, {1, -1}, {-1, 1}
	};

	for (int i = 0; i < 8; ++i) {
		int dx = directions[i][0];
		int dy = directions[i][1];

		int nx_minus1 = x - dx;
		int ny_minus1 = y - dy; //back (should be opponent)
		int nx_plus1 = x + dx;
		int ny_plus1 = y + dy; // front (should be stone)
		int nx_plus2 = x + (2 * dx);
		int ny_plus2 = y + (2 * dy); //2 grid in front (should be empty)

        //checking the ends for secure memory access
		if (nx_minus1 >= 0 && nx_minus1 < 19 && ny_minus1 >= 0 && ny_minus1 < 19 &&
			nx_plus2 >= 0 && nx_plus2 < 19 && ny_plus2 >= 0 && ny_plus2 < 19) {
			//searching for opponent, stone, stone, empty
			if (this->_grid[ny_minus1][nx_minus1] == opponent &&
				this->_grid[ny_plus1][nx_plus1] == stone &&
				this->_grid[ny_plus2][nx_plus2] == EMPTY) {
				return true; //alignement is actually breakable
			}
		}
	}
	return false; //alignement is not breakable
}
