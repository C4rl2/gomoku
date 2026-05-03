var Bridge = (function() {
  var G = null;

  function load(onReady) {
    GomokuModule().then(function(module) {
      G = module;
      onReady();
    }).catch(function(e) {
      document.body.textContent = 'Failed to load WASM: ' + e;
    });
  }

  function gameInit(depth, mode) { G.ccall('game_init', null, ['number', 'number'], [depth, mode]); }
  function placeStone(x, y)    { return G.ccall('place_stone', 'number', ['number', 'number'], [x, y]); }
  function aiPlay()            { return G.ccall('ai_play', 'number', [], []); }
  function getLastAiTime()     { return G.ccall('get_last_ai_time', 'number', [], []); }
  function getLastDepth()      { return G.ccall('get_last_depth', 'number', [], []); }
  function getCurrentPlayer()  { return G.ccall('get_current_player', 'number', [], []); }
  function getCaptures(player) { return G.ccall('get_captures', 'number', ['number'], [player]); }
  function isGameOver()        { return G.ccall('is_game_over', 'number', [], []) === 1; }
  function getWinner()         { return G.ccall('get_winner', 'number', [], []); }
  function getGameMode()       { return G.ccall('get_game_mode', 'number', [], []); }

  //runs the full ai pipeline for the current player without applying the move
  //returns {x, y} on success or null when no suggestion is available
  function suggestMove() {
    var packed = G.ccall('suggest_move', 'number', [], []);
    if (packed < 0) return null;
    return { x: packed % 19, y: Math.floor(packed / 19) };
  }
  function getLastSuggestTime() { return G.ccall('get_last_suggest_time', 'number', [], []); }

  //ai per-move debug counters
  function getLastNodes()        { return G.ccall('get_last_nodes',      'number', [], []); }
  function getLastCutoffs()      { return G.ccall('get_last_cutoffs',    'number', [], []); }
  function getLastTTHits()       { return G.ccall('get_last_tt_hits',    'number', [], []); }
  function getLastTHeuristic()   { return G.ccall('get_last_t_heuristic',  'number', [], []); }
  function getLastTMoveOrder()   { return G.ccall('get_last_t_move_order', 'number', [], []); }
  function getLastTZobrist()     { return G.ccall('get_last_t_zobrist',    'number', [], []); }
  function getLastTTT()          { return G.ccall('get_last_t_tt',         'number', [], []); }
  function getLastTTotal()       { return G.ccall('get_last_t_total',      'number', [], []); }

  function getBoard() {
    var ptr = G.ccall('get_board', 'number', [], []);
    return new Int32Array(G.HEAP32.buffer, ptr, 19 * 19);
  }

  return {
    load:               load,
    gameInit:           gameInit,
    placeStone:         placeStone,
    aiPlay:             aiPlay,
    getLastAiTime:      getLastAiTime,
    getLastDepth:       getLastDepth,
    getBoard:           getBoard,
    getCurrentPlayer:   getCurrentPlayer,
    getCaptures:        getCaptures,
    isGameOver:         isGameOver,
    getWinner:          getWinner,
    getLastNodes:       getLastNodes,
    getLastCutoffs:     getLastCutoffs,
    getLastTTHits:      getLastTTHits,
    getLastTHeuristic:  getLastTHeuristic,
    getLastTMoveOrder:  getLastTMoveOrder,
    getLastTZobrist:    getLastTZobrist,
    getLastTTT:         getLastTTT,
    getLastTTotal:      getLastTTotal,
    getGameMode:        getGameMode,
    suggestMove:        suggestMove,
    getLastSuggestTime: getLastSuggestTime
  };
})();