#ifndef GAME_HPP
# define GAME_HPP

# include "Board.hpp"
# include "AI.hpp"

// Replaced the original blocking run() loop with event-driven methods
// so the game can be driven by a web frontend (one call per user action).
class Game {
	private:
		Board	_board;
		AI		_ai;
		e_stone	_currentPlayer;
		bool	_gameOver;
		int		_winner; // 0=none, 1=BLACK, 2=WHITE, 3=draw

		void	_switchPlayer();
		int		_applyMove(int x, int y);

	public:
		Game();
		Game(const Game &other);
		Game &operator=(const Game &other);
		~Game();

		// Replaces the depth prompt from run() — called once before the first move.
		void	init(int depth);
		// Replaces the human input block in run() — validates rules then calls _applyMove.
		// Returns: 0=ok, -1=invalid, -2=double-three, -3=game already over
		int		placeStone(int x, int y);
		// Replaces the AI block in run() — computes best move and applies it.
		// Returns: 0=ok, -1=draw (board full), -3=game already over
		int		aiPlay(double &timeSpent);
		// Serializes the board to a flat int[361] array for the JS canvas renderer.
		void	getBoard(int *out) const;
		int		getCurrentPlayer() const;
		int		getCaptures(int player) const;
		bool	isGameOver() const;
		int		getWinner() const;
		int		getDepth() const;
};

#endif
