var Bridge = (function () {
  var G = null;
  var BOARD_SIZE = 19;
  var BOARD_CELLS = BOARD_SIZE * BOARD_SIZE;

  function load(onReady) {
    GomokuModule().then(function (module) {
      G = module;
      if (onReady) onReady();
    }).catch(function (e) {
      document.body.textContent = 'Failed to load WASM: ' + e;
    });
  }

  function call(name, types, args) {
    return G.ccall(name, 'number', types || [], args || []);
  }
  function noArg(name) { return function () { return call(name); }; }
  function oneArg(name) { return function (a) { return call(name, ['number'], [a]); }; }
  function twoArg(name) { return function (a, b) { return call(name, ['number', 'number'], [a, b]); }; }

  function int32View(ptr) {
    return new Int32Array(G.HEAP32.buffer, ptr, BOARD_CELLS);
  }

  var getHistoryLength = noArg('get_history_length');
  var getLastCapturedCount = noArg('get_last_captured_count');
  var getWinningLineCount = noArg('get_winning_line_count');
  var getHistoryMoveX = oneArg('get_history_move_x');
  var getHistoryMoveY = oneArg('get_history_move_y');
  var getHistoryMovePlayer = oneArg('get_history_move_player');
  var getLastCapturedX = oneArg('get_last_captured_x');
  var getLastCapturedY = oneArg('get_last_captured_y');
  var getWinningLineX = oneArg('get_winning_line_x');
  var getWinningLineY = oneArg('get_winning_line_y');

  function gameInit(depth, mode) {
    G.ccall('game_init', null, ['number', 'number'], [depth, mode]);
  }

  function suggestMove() {
    var packed = call('suggest_move');
    if (packed < 0) return null;
    return { x: packed % BOARD_SIZE, y: Math.floor(packed / BOARD_SIZE) };
  }

  function getMoveHistory(limit) {
    var len = getHistoryLength();
    var end = typeof limit === 'number' ? Math.min(limit, len - 1) : len - 1;
    var history = [];
    for (var i = 1; i <= end; i++) {
      history.push({
        x: getHistoryMoveX(i),
        y: getHistoryMoveY(i),
        player: getHistoryMovePlayer(i),
        index: i
      });
    }
    return history;
  }

  function getLastCapturedCells() {
    var cells = [];
    var count = getLastCapturedCount();
    for (var i = 0; i < count; i++)
      cells.push({ x: getLastCapturedX(i), y: getLastCapturedY(i) });
    return cells;
  }

  function getWinningLine() {
    var line = [];
    var count = getWinningLineCount();
    for (var i = 0; i < count; i++)
      line.push({ x: getWinningLineX(i), y: getWinningLineY(i) });
    return line;
  }

  return {
    load: load,
    gameInit: gameInit,
    placeStone: twoArg('place_stone'),
    undoMove: noArg('undo_move'),
    redoMove: noArg('redo_move'),
    gotoHistory: oneArg('goto_history'),
    aiPlay: noArg('ai_play'),
    suggestMove: suggestMove,
    isGameOver: function () { return call('is_game_over') === 1; },
    getCurrentPlayer: noArg('get_current_player'),
    getCaptures: oneArg('get_captures'),
    getWinner: noArg('get_winner'),
    getGameMode: noArg('get_game_mode'),
    getHistoryLength: getHistoryLength,
    getHistoryIndex: noArg('get_history_index'),
    getHistoryMoveX: getHistoryMoveX,
    getHistoryMoveY: getHistoryMoveY,
    getHistoryMovePlayer: getHistoryMovePlayer,
    getHistoryCapturedCount: oneArg('get_history_captured_count'),
    getHistoryCapturedX: twoArg('get_history_captured_x'),
    getHistoryCapturedY: twoArg('get_history_captured_y'),
    getLastCapturedCount: getLastCapturedCount,
    getLastCapturedX: getLastCapturedX,
    getLastCapturedY: getLastCapturedY,
    getLastCapturedCells: getLastCapturedCells,
    getWinningLineCount: getWinningLineCount,
    getWinningLineX: getWinningLineX,
    getWinningLineY: getWinningLineY,
    getWinningLine: getWinningLine,
    getMoveHistory: getMoveHistory,
    getLastAiTime: noArg('get_last_ai_time'),
    getLastSuggestTime: noArg('get_last_suggest_time'),
    getLastDepth: noArg('get_last_depth'),
    getLastNodes: noArg('get_last_nodes'),
    getLastCutoffs: noArg('get_last_cutoffs'),
    getLastTTHits: noArg('get_last_tt_hits'),
    getLastTHeuristic: noArg('get_last_t_heuristic'),
    getLastTMoveOrder: noArg('get_last_t_move_order'),
    getLastTZobrist: noArg('get_last_t_zobrist'),
    getLastTTT: noArg('get_last_t_tt'),
    getLastTTotal: noArg('get_last_t_total'),
    getBoard: function () { return int32View(call('get_board')); },
    getHistoryBoard: function (i) { return int32View(call('get_history_board', ['number'], [i])); }
  };
})();
