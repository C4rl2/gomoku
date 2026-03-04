#ifndef GAME_HPP
# define GAME_HPP

# include "Board.hpp"

class Game {
	private:
		Board	_board;
		e_stone	_currentPlayer;

		void	_switchPlayer();
		void	_printTurnInfo() const;

	public:
		Game();
		Game(const Game &other);
		Game &operator=(const Game &other);
		~Game();

		//main function that starts the loop
		void	run();
};

#endif