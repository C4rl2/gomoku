#ifndef BOARD_HPP
# define BOARD_HPP

# include <iostream>

//possible states of a intersection of the board
enum e_stone {
	EMPTY = 0,
	BLACK = 1,
	WHITE = 2
};

class Board {
	private:
		e_stone _grid[19][19];

		int		_countDirection(int x, int y, int dx, int dy, e_stone stone) const;
		bool	_isVulnerable(int x, int y, e_stone stone) const;

	public:
		Board();
		Board(const Board &other);
		Board &operator=(const Board &other);
		~Board();

		e_stone	getStone(int x, int y) const;
		bool	setStone(int x, int y, e_stone stone);

		void	printBoard() const;

		int		executeCaptures(int x, int y, e_stone stone);
		bool	checkWin(int x, int y, e_stone stone) const;
};

#endif