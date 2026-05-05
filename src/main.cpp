#include "Game.hpp"

#ifdef __EMSCRIPTEN__
# include <emscripten/emscripten.h>
#else
# define EMSCRIPTEN_KEEPALIVE
#endif

static Game   g_game;
static int    g_board_buf[19 * 19];
static int    g_history_board_buf[19 * 19];
static double g_last_ai_time      = 0.0;
static double g_last_suggest_time = 0.0;
static int    g_suggested_x       = -1;
static int    g_suggested_y       = -1;

extern "C" {
	EMSCRIPTEN_KEEPALIVE void	game_init(int depth, int mode) { g_game.init(depth, mode); g_suggested_x = -1; g_suggested_y = -1; g_last_suggest_time = 0.0; }
	EMSCRIPTEN_KEEPALIVE int	place_stone(int x, int y) { return g_game.placeStone(x, y); }
	EMSCRIPTEN_KEEPALIVE int	undo_move()               { return g_game.undoMove(); }
	EMSCRIPTEN_KEEPALIVE int	redo_move()               { return g_game.redoMove(); }
	EMSCRIPTEN_KEEPALIVE int	goto_history(int index)   { return g_game.gotoHistory(index); }
	EMSCRIPTEN_KEEPALIVE int	ai_play()                 { return g_game.aiPlay(g_last_ai_time); }
	EMSCRIPTEN_KEEPALIVE double	get_last_ai_time()        { return g_last_ai_time; }
	EMSCRIPTEN_KEEPALIVE int*	get_board()               { g_game.getBoard(g_board_buf); return g_board_buf; }
	EMSCRIPTEN_KEEPALIVE int*	get_history_board(int index) { g_game.getHistoryBoard(index, g_history_board_buf); return g_history_board_buf; }
	EMSCRIPTEN_KEEPALIVE int	get_current_player()      { return g_game.getCurrentPlayer(); }
	EMSCRIPTEN_KEEPALIVE int	get_captures(int player)  { return g_game.getCaptures(player); }
	EMSCRIPTEN_KEEPALIVE int	is_game_over()            { return g_game.isGameOver() ? 1 : 0; }
	EMSCRIPTEN_KEEPALIVE int	get_winner()              { return g_game.getWinner(); }
	EMSCRIPTEN_KEEPALIVE int	get_last_depth()          { return g_game.getLastDepth(); }
	EMSCRIPTEN_KEEPALIVE int	get_game_mode()           { return g_game.getGameMode(); }
	EMSCRIPTEN_KEEPALIVE int	get_history_length()      { return g_game.getHistoryLength(); }
	EMSCRIPTEN_KEEPALIVE int	get_history_index()       { return g_game.getHistoryIndex(); }
	EMSCRIPTEN_KEEPALIVE int	get_history_move_x(int index) { return g_game.getHistoryMoveX(index); }
	EMSCRIPTEN_KEEPALIVE int	get_history_move_y(int index) { return g_game.getHistoryMoveY(index); }
	EMSCRIPTEN_KEEPALIVE int	get_history_move_player(int index) { return g_game.getHistoryMovePlayer(index); }
	EMSCRIPTEN_KEEPALIVE int	get_last_captured_count() { return g_game.getLastCapturedCount(); }
	EMSCRIPTEN_KEEPALIVE int	get_last_captured_x(int index) { return g_game.getLastCapturedX(index); }
	EMSCRIPTEN_KEEPALIVE int	get_last_captured_y(int index) { return g_game.getLastCapturedY(index); }
	EMSCRIPTEN_KEEPALIVE int	get_history_captured_count(int historyIndex) { return g_game.getHistoryCapturedCount(historyIndex); }
	EMSCRIPTEN_KEEPALIVE int	get_history_captured_x(int historyIndex, int capturedIndex) { return g_game.getHistoryCapturedX(historyIndex, capturedIndex); }
	EMSCRIPTEN_KEEPALIVE int	get_history_captured_y(int historyIndex, int capturedIndex) { return g_game.getHistoryCapturedY(historyIndex, capturedIndex); }
	EMSCRIPTEN_KEEPALIVE int	get_winning_line_count() { return g_game.getWinningLineCount(); }
	EMSCRIPTEN_KEEPALIVE int	get_winning_line_x(int index) { return g_game.getWinningLineX(index); }
	EMSCRIPTEN_KEEPALIVE int	get_winning_line_y(int index) { return g_game.getWinningLineY(index); }

	//runs the full ai pipeline for the current player without applying the move
	//returns y * 19 + x on success, -1 if no suggestion (game over or wrong mode)
	EMSCRIPTEN_KEEPALIVE int	suggest_move() {
		Move m = g_game.suggestMove(g_last_suggest_time);
		g_suggested_x = m.x;
		g_suggested_y = m.y;
		if (m.x < 0 || m.y < 0)
			return -1;
		return m.y * 19 + m.x;
	}
	EMSCRIPTEN_KEEPALIVE int	get_suggested_x()       { return g_suggested_x; }
	EMSCRIPTEN_KEEPALIVE int	get_suggested_y()       { return g_suggested_y; }
	EMSCRIPTEN_KEEPALIVE double	get_last_suggest_time() { return g_last_suggest_time; }

	EMSCRIPTEN_KEEPALIVE int	get_last_nodes()          { return g_game.getLastNodes(); }
	EMSCRIPTEN_KEEPALIVE int	get_last_cutoffs()        { return g_game.getLastCutoffs(); }
	EMSCRIPTEN_KEEPALIVE int	get_last_tt_hits()        { return g_game.getLastTTHits(); }
	EMSCRIPTEN_KEEPALIVE double	get_last_t_heuristic()    { return g_game.getLastTimeHeuristic(); }
	EMSCRIPTEN_KEEPALIVE double	get_last_t_move_order()   { return g_game.getLastTimeMoveOrdering(); }
	EMSCRIPTEN_KEEPALIVE double	get_last_t_zobrist()      { return g_game.getLastTimeZobrist(); }
	EMSCRIPTEN_KEEPALIVE double	get_last_t_tt()           { return g_game.getLastTimeTT(); }
	EMSCRIPTEN_KEEPALIVE double	get_last_t_total()        { return g_game.getLastTimeTotal(); }
}

int main() { return 0; }
