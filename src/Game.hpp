#ifndef GAME_HPP
# define GAME_HPP

# include <vector>
# include "Board.hpp"
# include "AI.hpp"
# include "MoveRecord.hpp"

//game mode selected from the home screen
enum e_game_mode {
	MODE_AI          = 0, //human BLACK vs ai WHITE
	MODE_HVH         = 1, //human vs human, rules only
	MODE_HVH_SUGGEST = 2  //human vs human with ai suggestion per turn
};

// Replaced the original blocking run() loop with event-driven methods
// so the game can be driven by a web frontend (one call per user action).
class Game {
	private:
		Board		_board;
		AI			_ai;
		e_stone		_currentPlayer;
		bool		_gameOver;
		int			_winner; // 0=none, 1=BLACK, 2=WHITE, 3=draw
		e_game_mode	_gameMode;
		std::vector<MoveRecord>	_history;

		void	_switchPlayer();
		int		_applyMove(int x, int y);

	public:
		Game();
		Game(const Game &other);
		Game &operator=(const Game &other);
		~Game();

		// Replaces the depth prompt from run() — called once before the first move.
		void	init(int depth, int mode);
		// Replaces the human input block in run() — validates rules then calls _applyMove.
		// Returns: 0=ok, -1=invalid, -2=double-three, -3=game already over
		int		placeStone(int x, int y);
		//restores the state saved before the last successful move
		//returns: 0=ok, -1=history empty
		int		undoMove();
		// Replaces the AI block in run() — computes best move and applies it.
		// Returns: 0=ok, -1=draw (board full), -3=game already over (or non-AI mode)
		int		aiPlay(double &timeSpent);
		//computes a move via the full ai pipeline without applying it
		//returns Move{-1,-1,0} if game over or mode != MODE_HVH_SUGGEST
		Move	suggestMove(double &timeSpent);
		// Serializes the board to a flat int[361] array for the JS canvas renderer.
		void	getBoard(int *out) const;
		int		getCurrentPlayer() const;
		int		getCaptures(int player) const;
		bool	isGameOver() const;
		int		getWinner() const;
		int		getDepth() const;
		int		getLastDepth() const;
		int		getGameMode() const;

		//ai per-move instrumentation forwarded from the underlying AI instance
		int		getLastNodes() const;
		int		getLastCutoffs() const;
		int		getLastTTHits() const;
		double	getLastTimeHeuristic() const;
		double	getLastTimeMoveOrdering() const;
		double	getLastTimeZobrist() const;
		double	getLastTimeTT() const;
		double	getLastTimeTotal() const;
};

#endif