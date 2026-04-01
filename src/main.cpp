#include "Game.hpp"

#ifdef __EMSCRIPTEN__
# include <emscripten/emscripten.h>
#else
# define EMSCRIPTEN_KEEPALIVE
#endif

static Game   g_game;
static int    g_board_buf[19 * 19];
static double g_last_ai_time = 0.0;

extern "C" {
	EMSCRIPTEN_KEEPALIVE void	game_init(int depth)      { g_game.init(depth); }
	EMSCRIPTEN_KEEPALIVE int	place_stone(int x, int y) { return g_game.placeStone(x, y); }
	EMSCRIPTEN_KEEPALIVE int	ai_play()                 { return g_game.aiPlay(g_last_ai_time); }
	EMSCRIPTEN_KEEPALIVE double	get_last_ai_time()        { return g_last_ai_time; }
	EMSCRIPTEN_KEEPALIVE int*	get_board()               { g_game.getBoard(g_board_buf); return g_board_buf; }
	EMSCRIPTEN_KEEPALIVE int	get_current_player()      { return g_game.getCurrentPlayer(); }
	EMSCRIPTEN_KEEPALIVE int	get_captures(int player)  { return g_game.getCaptures(player); }
	EMSCRIPTEN_KEEPALIVE int	is_game_over()            { return g_game.isGameOver() ? 1 : 0; }
	EMSCRIPTEN_KEEPALIVE int	get_winner()              { return g_game.getWinner(); }
}

int main() { return 0; }
