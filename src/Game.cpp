#include "Game.hpp"
#include <ctime>

Game::Game() : _ai(WHITE), _currentPlayer(BLACK), _gameOver(false), _winner(0) {}

Game::Game(const Game &other) {
	*this = other;
}

Game &Game::operator=(const Game &other) {
	if (this != &other) {
		this->_board         = other._board;
		this->_ai            = other._ai;
		this->_currentPlayer = other._currentPlayer;
		this->_gameOver      = other._gameOver;
		this->_winner        = other._winner;
	}
	return *this;
}

Game::~Game() {}

void Game::_switchPlayer() {
	if (this->_currentPlayer == BLACK)
		this->_currentPlayer = WHITE;
	else
		this->_currentPlayer = BLACK;
}

// Replaces the depth prompt loop from run().
void Game::init(int depth) {
	this->_board         = Board();
	this->_ai            = AI(WHITE);
	this->_ai.setDepth(depth);
	this->_currentPlayer = BLACK;
	this->_gameOver      = false;
	this->_winner        = 0;
}

// Extracted from the setStone block inside run()'s while loop.
// Shared by placeStone() and aiPlay() to avoid duplication.
int Game::_applyMove(int x, int y) {
	if (!this->_board.setStone(x, y, this->_currentPlayer))
		return -1;

	this->_board.executeCaptures(x, y, this->_currentPlayer);

	// Check if the opponent's existing five-in-a-row survived the captures.
	e_stone opponent = (this->_currentPlayer == BLACK) ? WHITE : BLACK;
	if (this->_board.hasFive(opponent)) {
		this->_gameOver = true;
		this->_winner   = (int)opponent;
		return 0;
	}

	e_win_state winState     = this->_board.checkWin(x, y, this->_currentPlayer);
	bool        winByCapture = (this->_board.getCaptures(this->_currentPlayer) >= 5);

	if (winState == WIN || winByCapture) {
		this->_gameOver = true;
		this->_winner   = (int)this->_currentPlayer;
		return 0;
	}

	this->_switchPlayer();
	if (winState == BREAKABLE_FIVE)
		return 2; // game continues, opponent can still break the alignment
	return 0;
}

// Replaces the human input validation block from run().
int Game::placeStone(int x, int y) {
	if (this->_gameOver)
		return -3;
	if (x < 0 || x >= 19 || y < 0 || y >= 19 || this->_board.getStone(x, y) != EMPTY)
		return -1;
	if (this->_board.isDoubleThree(x, y, this->_currentPlayer) &&
		this->_board.willCapture(x, y, this->_currentPlayer) == false)
		return -2;

	return this->_applyMove(x, y);
}

// Replaces the AI block from run(). timeSpent is displayed by the frontend
// (the subject requires showing the AI computation time).
int Game::aiPlay(double &timeSpent) {
	if (this->_gameOver)
		return -3;

	clock_t start  = clock();
	Move    aiMove = this->_ai.getBestMove(this->_board);
	clock_t end    = clock();
	timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

	if (aiMove.x == -1 && aiMove.y == -1) {
		this->_gameOver = true;
		this->_winner   = 3;
		return -1;
	}

	return this->_applyMove(aiMove.x, aiMove.y);
}

// Serializes the grid to a flat array for the JS canvas renderer.
void Game::getBoard(int *out) const {
	for (int y = 0; y < 19; ++y)
		for (int x = 0; x < 19; ++x)
			out[y * 19 + x] = (int)this->_board.getStone(x, y);
}

int Game::getCurrentPlayer() const {
	return (int)this->_currentPlayer;
}

int Game::getCaptures(int player) const {
	return this->_board.getCaptures((e_stone)player);
}

bool Game::isGameOver() const {
	return this->_gameOver;
}

int Game::getWinner() const {
	return this->_winner;
}

int Game::getDepth() const {
	return this->_ai.getDepth();
}
