#ifndef GAME_HPP
# define GAME_HPP

# include "Board.hpp"
# include "AI.hpp"

class Game {
	private:
		Board	_board;
		AI		_ai;
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