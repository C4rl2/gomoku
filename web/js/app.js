var aiThinking = false;
var currentMode = 0;
var currentSuggestion = null;
var moveHistory = [];

var MODE_AI = 0;
var MODE_HVH = 1;
var MODE_HVH_SUGGEST = 2;

document.addEventListener('DOMContentLoaded', function() {
  var themeToggle = document.getElementById('theme-toggle');

  if (!themeToggle) {
    return;
  }

  themeToggle.addEventListener('change', function(e) {
    if (e.target.checked) {
      document.documentElement.setAttribute('data-theme', 'light');
    } else {
      document.documentElement.removeAttribute('data-theme');
    }
    
    if (document.getElementById('board') && document.getElementById('game').style.display !== 'none' && Bridge.getBoard) {
      Render.board(Bridge.getBoard(), moveHistory);
    }
  });
});

Bridge.load(function() {
  document.getElementById('start-btn').disabled = false;
});

function startGame() {
  var depth = Math.max(1, Math.min(10, parseInt(document.getElementById('depth').value) || 5));
  var modeRadio = document.querySelector('input[name="mode"]:checked');
  currentMode = modeRadio ? parseInt(modeRadio.value, 10) : 0;
  currentSuggestion = null;
  moveHistory = [];
  Bridge.gameInit(depth, currentMode);

  document.getElementById('content').style.display = 'none';
  document.getElementById('setup').style.display = 'none';
  document.getElementById('game').style.display  = '';

  var canvas = document.getElementById('board');
  Render.init(canvas);
  canvas.addEventListener('click', onCanvasClick);

  Render.board(Bridge.getBoard(), moveHistory);
  setStatus('Blue to play');

  if (currentMode === MODE_HVH_SUGGEST)
    refreshSuggestion();
}

function onCanvasClick(evt) {
  if (aiThinking || Bridge.isGameOver()) return;
  //in mode AI only the black human can click; in HvH modes both players click
  if (currentMode === MODE_AI && Bridge.getCurrentPlayer() !== 1) return;

  var cell = Render.cellFromClick(evt);
  if (!cell) return;

  var result = Bridge.placeStone(cell.x, cell.y);
  if (result === -1) { setStatus('Invalid move'); return; }
  if (result === -2) { setStatus('Double-three forbidden!'); return; }

  moveHistory.push({ x: cell.x, y: cell.y });
  //a successful move invalidates the previous suggestion
  currentSuggestion = null;
  Render.board(Bridge.getBoard(), moveHistory);
  updateCaptures();
  if (checkGameOver()) return;

  if (currentMode === MODE_AI) {
    aiThinking = true;
    setStatus('AI thinking...');
    setTimeout(runAi, 20);
  } else if (currentMode === MODE_HVH) {
    setStatus(Bridge.getCurrentPlayer() === 1 ? 'Blue to play' : 'Pink to play');
  } else { //MODE_HVH_SUGGEST
    setStatus(Bridge.getCurrentPlayer() === 1 ? 'Blue to play' : 'Pink to play');
    refreshSuggestion();
  }
}

function onUndoClick() {
  if (aiThinking) return;
  if (Bridge.undoMove() !== 0) return;

  moveHistory.pop();
  //in HvAI, if we just undid the AI move, also undo the human move that preceded it
  if (currentMode === MODE_AI && Bridge.getCurrentPlayer() === 2) {
    Bridge.undoMove();
    moveHistory.pop();
  }

  currentSuggestion = null;
  Render.board(Bridge.getBoard(), moveHistory);
  updateCaptures();
  setStatus(Bridge.getCurrentPlayer() === 1 ? 'Blue to play' : 'Pink to play');

  if (currentMode === MODE_HVH_SUGGEST) refreshSuggestion();
}

function refreshSuggestion() {
  var label = document.getElementById('suggestion');
  if (Bridge.isGameOver()) {
    currentSuggestion = null;
    if (label) label.textContent = '';
    return;
  }

  aiThinking = true;
  setStatus('AI suggesting...');
  setTimeout(function() {
    currentSuggestion = Bridge.suggestMove();
    aiThinking = false;

    var t = Bridge.getLastSuggestTime();
    document.getElementById('ai-time').textContent  = 'AI time: ' + t.toFixed(3) + 's';
    document.getElementById('ai-depth').textContent = 'AI depth reached: ' + Bridge.getLastDepth();
    updateDebugPanel();

    Render.board(Bridge.getBoard(), moveHistory);
    if (Render.suggestion) Render.suggestion(currentSuggestion);

    if (label) {
      if (currentSuggestion)
        label.textContent = 'Suggestion: (' + currentSuggestion.x + ', ' + currentSuggestion.y + ')';
      else
        label.textContent = '';
    }

    setStatus(Bridge.getCurrentPlayer() === 1 ? 'Blue to play' : 'Pink to play');
  }, 20);
}

function runAi() {
  var prevBoard = new Int32Array(Bridge.getBoard());
  var result = Bridge.aiPlay();
  var t      = Bridge.getLastAiTime();
  aiThinking = false;

  var newBoard = Bridge.getBoard();
  for (var i = 0; i < 19 * 19; i++) {
    if (prevBoard[i] === 0 && newBoard[i] === 2) {
      moveHistory.push({ x: i % 19, y: Math.floor(i / 19) });
      break;
    }
  }

  Render.board(Bridge.getBoard(), moveHistory);
  updateCaptures();
  document.getElementById('ai-time').textContent  = 'AI time: ' + t.toFixed(3) + 's';
  document.getElementById('ai-depth').textContent = 'AI depth reached: ' + Bridge.getLastDepth();
  updateDebugPanel();

  if (result === -1) { setStatus('Draw — board full'); return; }
  if (checkGameOver()) return;
  setStatus('Blue to play');
}

function updateDebugPanel() {
  var total    = Bridge.getLastTTotal();
  var heur     = Bridge.getLastTHeuristic();
  var order    = Bridge.getLastTMoveOrder();
  var zobrist  = Bridge.getLastTZobrist();
  var tt       = Bridge.getLastTTT();
  var coreMM   = Math.max(0, total - (heur + order + zobrist + tt));

  document.getElementById('dbg-id').textContent       = total.toFixed(2);
  document.getElementById('dbg-eval').textContent     = heur.toFixed(2);
  document.getElementById('dbg-order').textContent    = order.toFixed(2);
  document.getElementById('dbg-zobrist').textContent  = zobrist.toFixed(3);
  document.getElementById('dbg-tt').textContent       = tt.toFixed(2);
  document.getElementById('dbg-mm').textContent       = coreMM.toFixed(2);
  document.getElementById('dbg-cutoffs').textContent  = Bridge.getLastCutoffs();
  document.getElementById('dbg-tthits').textContent   = Bridge.getLastTTHits();
  document.getElementById('dbg-nodes').textContent    = Bridge.getLastNodes();
}

function checkGameOver() {
  if (!Bridge.isGameOver()) return false;
  var w     = Bridge.getWinner();
  var names = { 1: 'Blue', 2: 'Pink', 3: 'Draw' };
  setStatus(w === 3 ? 'Draw!' : names[w] + ' wins!');
  return true;
}

function updateCaptures() {
  document.getElementById('cap-black').textContent = Bridge.getCaptures(1);
  document.getElementById('cap-white').textContent = Bridge.getCaptures(2);
}

function setStatus(msg) {
  document.getElementById('status').textContent = msg;
}
