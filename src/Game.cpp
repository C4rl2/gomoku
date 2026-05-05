#include "Game.hpp"
#include <ctime>

Game::Game()
	: _ai(WHITE), _currentPlayer(BLACK), _gameOver(false), _winner(0),
	_gameMode(MODE_AI), _historyIndex(0) {}

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
		this->_gameMode      = other._gameMode;
		this->_history       = other._history;
		this->_historyIndex  = other._historyIndex;
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
void Game::init(int depth, int mode) {
	this->_board         = Board();
	this->_ai            = AI(WHITE);
	this->_ai.setDepth(depth);
	this->_currentPlayer = BLACK;
	this->_gameOver      = false;
	this->_winner        = 0;
	this->_gameMode      = (e_game_mode)mode;
	this->_history.clear();
	this->_historyIndex  = 0;
	this->_saveHistoryState(-1, -1, EMPTY);
}

void Game::_saveHistoryState(int moveX, int moveY, e_stone playedStone) {
	if (this->_historyIndex + 1 < (int)this->_history.size())
		this->_history.erase(this->_history.begin() + this->_historyIndex + 1, this->_history.end());
	this->_history.push_back(MoveRecord(
		this->_board,
		this->_currentPlayer,
		this->_gameOver,
		this->_winner,
		moveX,
		moveY,
		playedStone
	));
	this->_historyIndex = (int)this->_history.size() - 1;
}

void Game::_restoreHistoryState(int index) {
	const MoveRecord &rec = this->_history[index];
	this->_board         = rec._board;
	this->_currentPlayer = rec._currentPlayer;
	this->_gameOver      = rec._gameOver;
	this->_winner        = rec._winner;
	this->_historyIndex  = index;
}

// Extracted from the setStone block inside run()'s while loop.
// Shared by placeStone() and aiPlay() to avoid duplication.
int Game::_applyMove(int x, int y) {
	e_stone playedStone = this->_currentPlayer;
	this->_board.clearLastWinningLine();

	if (!this->_board.setStone(x, y, this->_currentPlayer))
		return -1;

	this->_board.executeCaptures(x, y, this->_currentPlayer);

	// Check if the opponent's existing five-in-a-row survived the captures.
	e_stone opponent = (this->_currentPlayer == BLACK) ? WHITE : BLACK;
	if (this->_board.hasFive(opponent)) {
		this->_board.findAnyFiveLine(opponent);
		this->_gameOver = true;
		this->_winner   = (int)opponent;
		this->_saveHistoryState(x, y, playedStone);
		return 0;
	}

	e_win_state winState     = this->_board.checkWin(x, y, this->_currentPlayer);
	bool        winByCapture = (this->_board.getCaptures(this->_currentPlayer) >= 5);

	if (winState == WIN || winByCapture) {
		if (winState == WIN)
			this->_board.findWinningLineForMove(x, y, this->_currentPlayer, true);
		else
			this->_board.clearLastWinningLine();
		this->_gameOver = true;
		this->_winner   = (int)this->_currentPlayer;
		this->_saveHistoryState(x, y, playedStone);
		return 0;
	}

	this->_switchPlayer();
	if (winState == BREAKABLE_FIVE)
		this->_board.findWinningLineForMove(x, y, playedStone, false);
	this->_saveHistoryState(x, y, playedStone);
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

//moves the current state cursor one snapshot backward without destroying history
int Game::undoMove() {
	if (this->_historyIndex <= 0)
		return -1;
	this->_restoreHistoryState(this->_historyIndex - 1);
	return 0;
}

int Game::redoMove() {
	if (this->_historyIndex + 1 >= (int)this->_history.size())
		return -1;
	this->_restoreHistoryState(this->_historyIndex + 1);
	return 0;
}

int Game::gotoHistory(int index) {
	if (index < 0 || index >= (int)this->_history.size())
		return -1;
	this->_restoreHistoryState(index);
	return 0;
}

// Replaces the AI block from run(). timeSpent is displayed by the frontend
// (the subject requires showing the AI computation time).
int Game::aiPlay(double &timeSpent) {
	if (this->_gameOver || (this->_gameMode != MODE_AI && this->_gameMode != MODE_AI_V_AI))
		return -3;

	this->_ai.setAiTeam(this->_currentPlayer);
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

//runs the full ai pipeline (minimax / ab / zobrist / tt / id / move ordering)
//for the current player without mutating the game state, so the suggestion
//can be displayed and either followed or ignored by the human player
Move Game::suggestMove(double &timeSpent) {
	Move none = {-1, -1, 0};
	timeSpent = 0.0;
	if (this->_gameOver)
		return none;

	//rebind ai team to the current player so minimax maximises for them
	//setAiTeam clears the tt when the team actually changes
	this->_ai.setAiTeam(this->_currentPlayer);

	clock_t start = clock();
	Move    m     = this->_ai.getBestMove(this->_board);
	clock_t end   = clock();
	timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
	return m;
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

int Game::getLastDepth() const {
	return this->_ai.getLastDepth();
}

int Game::getGameMode() const {
	return (int)this->_gameMode;
}

int Game::getHistoryLength() const {
	return (int)this->_history.size();
}

int Game::getHistoryIndex() const {
	return this->_historyIndex;
}

int Game::getHistoryMoveX(int index) const {
	if (index <= 0 || index >= (int)this->_history.size())
		return -1;
	return this->_history[index]._moveX;
}

int Game::getHistoryMoveY(int index) const {
	if (index <= 0 || index >= (int)this->_history.size())
		return -1;
	return this->_history[index]._moveY;
}

int Game::getHistoryMovePlayer(int index) const {
	if (index <= 0 || index >= (int)this->_history.size())
		return 0;
	return (int)this->_history[index]._playedStone;
}

void Game::getHistoryBoard(int index, int *out) const {
	const Board *board = &this->_board;
	if (index >= 0 && index < (int)this->_history.size())
		board = &this->_history[index]._board;
	for (int y = 0; y < 19; ++y)
		for (int x = 0; x < 19; ++x)
			out[y * 19 + x] = (int)board->getStone(x, y);
}

int Game::getLastCapturedCount() const {
	return this->_board.getLastCapturedCount();
}

int Game::getLastCapturedX(int index) const {
	return this->_board.getLastCapturedX(index);
}

int Game::getLastCapturedY(int index) const {
	return this->_board.getLastCapturedY(index);
}

int Game::getHistoryCapturedCount(int historyIndex) const {
	if (historyIndex <= 0 || historyIndex >= (int)this->_history.size())
		return 0;
	return this->_history[historyIndex]._board.getLastCapturedCount();
}

int Game::getHistoryCapturedX(int historyIndex, int capturedIndex) const {
	if (historyIndex <= 0 || historyIndex >= (int)this->_history.size())
		return -1;
	return this->_history[historyIndex]._board.getLastCapturedX(capturedIndex);
}

int Game::getHistoryCapturedY(int historyIndex, int capturedIndex) const {
	if (historyIndex <= 0 || historyIndex >= (int)this->_history.size())
		return -1;
	return this->_history[historyIndex]._board.getLastCapturedY(capturedIndex);
}

int Game::getWinningLineCount() const {
	return this->_board.getLastWinningCount();
}

int Game::getWinningLineX(int index) const {
	return this->_board.getLastWinningX(index);
}

int Game::getWinningLineY(int index) const {
	return this->_board.getLastWinningY(index);
}

//forward instrumentation getters for the frontend debug panel
int Game::getLastNodes() const               { return this->_ai.getLastNodes(); }
int Game::getLastCutoffs() const             { return this->_ai.getLastCutoffs(); }
int Game::getLastTTHits() const              { return this->_ai.getLastTTHits(); }
double Game::getLastTimeHeuristic() const    { return this->_ai.getLastTimeHeuristic(); }
double Game::getLastTimeMoveOrdering() const { return this->_ai.getLastTimeMoveOrdering(); }
double Game::getLastTimeZobrist() const      { return this->_ai.getLastTimeZobrist(); }
double Game::getLastTimeTT() const           { return this->_ai.getLastTimeTT(); }
double Game::getLastTimeTotal() const        { return this->_ai.getLastTimeTotal(); }
