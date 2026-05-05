#ifndef BOARD_HPP
# define BOARD_HPP

# include <iostream>

//possible states of a intersection of the board
enum e_stone {
	EMPTY = 0,
	BLACK = 1,
	WHITE = 2
};

enum e_win_state {
	NONE = 0,
	WIN = 1,
	BREAKABLE_FIVE = 2
};

class Board {
	private:
		enum { MAX_CAPTURED_STONES = 16 };
		enum { WINNING_LINE_SIZE = 5 };

		e_stone _grid[19][19];
		int		_capturesBlack;
		int		_capturesWhite;
		int		_lastCapturedCount;
		int		_lastCapturedX[MAX_CAPTURED_STONES];
		int		_lastCapturedY[MAX_CAPTURED_STONES];
		int		_lastWinningCount;
		int		_lastWinningX[WINNING_LINE_SIZE];
		int		_lastWinningY[WINNING_LINE_SIZE];

		int		_countDirection(int x, int y, int dx, int dy, e_stone stone) const;
		bool	_isVulnerable(int x, int y, e_stone stone) const;
		int		_getRelative(int x, int y, int i, int dx, int dy, e_stone stone) const;
		bool	_isFreeThreeInDir(int x, int y, int dx, int dy, e_stone stone) const;
		void	_clearLastCaptured();
		void	_recordCapturedStone(int x, int y);
		bool	_isLineBreakable(int x, int y, int dx, int dy, int countForward, int countBackward, e_stone stone) const;
		void	_storeLineFromRun(int x, int y, int dx, int dy, int countForward, int countBackward);

	public:
		Board();
		Board(const Board &other);
		Board &operator=(const Board &other);
		~Board();

		e_stone			getStone(int x, int y) const;
		bool			setStone(int x, int y, e_stone stone);
		int				getCaptures(e_stone stone) const;
		int				getLastCapturedCount() const;
		int				getLastCapturedX(int index) const;
		int				getLastCapturedY(int index) const;
		int				getLastWinningCount() const;
		int				getLastWinningX(int index) const;
		int				getLastWinningY(int index) const;

		void			printBoard() const;

		int				executeCaptures(int x, int y, e_stone stone);
		void			clearLastWinningLine();
		bool			findWinningLineForMove(int x, int y, e_stone stone, bool requireUnbreakable);
		bool			findAnyFiveLine(e_stone stone);
		e_win_state		checkWin(int x, int y, e_stone stone) const;
		bool			hasFive(e_stone stone) const;
		bool			isDoubleThree(int x, int y, e_stone stone) const;
		bool			willCapture(int x, int y, e_stone stone) const;
};

#endif
