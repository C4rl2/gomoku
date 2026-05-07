var SETUP_EXIT_MS = 280;
var HINT_DELAY_MS = 20;
var HUMAN_AI_DELAY_MS = 20;
var AI_V_AI_DELAY_MS = 180;

Bridge.load();

function startGame() {
  var depth = Math.max(1, Math.min(10, parseInt(document.getElementById('depth').value) || 5));
  var modeRadio = document.querySelector('input[name="mode"]:checked');
  currentMode = modeRadio ? parseInt(modeRadio.value, 10) : 0;
  currentSuggestion = null;
  hintsEnabled = document.getElementById('hint-toggle').checked;
  moveHistory = [];
  previewHistoryIndex = null;
  closeWinModal();
  Render.clearWinHighlights();
  Bridge.gameInit(depth, currentMode);

  document.getElementById('start-btn').disabled = true;
  document.getElementById('setup').classList.add('setup--leaving');
  setTimeout(enterGameView, SETUP_EXIT_MS);
}

function enterGameView() {
  document.getElementById('content').style.display = 'none';
  document.getElementById('setup').style.display = 'none';
  var gameEl = document.getElementById('game');
  gameEl.style.display = '';
  gameEl.classList.add('game-entering');

  var canvas = document.getElementById('board');
  Render.init(canvas);
  canvas.addEventListener('click', onCanvasClick);

  syncMoveHistory();
  renderCurrentBoard();
  setStatus('');
  updateTurnIndicator(1);
  updateLives(5, 5);
  updateActionButtons();
  renderMoveHistory();

  if (currentMode === MODE_AI_V_AI)
    scheduleAiTurn();
  else
    maybeRefreshHint();
}

function onCanvasClick(evt) {
  if (aiThinking || Bridge.isGameOver()) return;
  if (currentMode === MODE_AI_V_AI) return;
  if (currentMode === MODE_HUMAN_AI && Bridge.getCurrentPlayer() !== 1) return;

  var cell = Render.cellFromClick(evt);
  if (!cell) return;

  var result = Bridge.placeStone(cell.x, cell.y);
  if (result === -1) { setStatus('Invalid move'); return; }
  if (result === -2) { setStatus('Double-three forbidden!'); return; }

  currentSuggestion = null;
  finishAppliedMove(result);
}

function onUndoClick() {
  if (aiThinking) return;
  closeWinModal();
  if (Bridge.undoMove() !== 0) {
    updateActionButtons();
    return;
  }

  if (currentMode === MODE_HUMAN_AI && Bridge.getCurrentPlayer() === 2)
    Bridge.undoMove();

  currentSuggestion = null;
  syncMoveHistory();
  renderCurrentBoard();
  updateCaptures();
  updateTurnIndicator(Bridge.getCurrentPlayer());
  setStatus('');
  updateActionButtons();
  renderMoveHistory();

  maybeRefreshHint();
}

function refreshSuggestion() {
  var label = document.getElementById('suggestion');
  if (Bridge.isGameOver()) {
    currentSuggestion = null;
    label.textContent = '';
    return;
  }

  currentSuggestion = null;
  aiThinking = true;
  setStatus('Hint thinking...');
  updateActionButtons();
  setTimeout(function () {
    currentSuggestion = Bridge.suggestMove();
    aiThinking = false;

    updateAiStats(Bridge.getLastSuggestTime(), Bridge.getLastDepth());
    updateDebugPanel();

    renderCurrentBoard();
    Render.suggestion(currentSuggestion);

    label.textContent = currentSuggestion
      ? 'Suggestion: (' + currentSuggestion.x + ', ' + currentSuggestion.y + ')'
      : '';

    setStatus(Bridge.getCurrentPlayer() === 1 ? 'Blue to play' : 'Pink to play');
    updateActionButtons();
  }, HINT_DELAY_MS);
}

function maybeRefreshHint() {
  if (hintsEnabled && !Bridge.isGameOver())
    refreshSuggestion();
}

function onHintClick() {
  if (aiThinking || Bridge.isGameOver()) return;
  refreshSuggestion();
}

function scheduleAiTurn() {
  if (Bridge.isGameOver()) return;
  aiThinking = true;
  setStatus('AI thinking...');
  updateTurnIndicator(Bridge.getCurrentPlayer());
  updateActionButtons();
  setTimeout(runAi, currentMode === MODE_AI_V_AI ? AI_V_AI_DELAY_MS : HUMAN_AI_DELAY_MS);
}

function runAi() {
  var result = Bridge.aiPlay();
  aiThinking = false;

  updateAiStats(Bridge.getLastAiTime(), Bridge.getLastDepth());
  updateDebugPanel();
  finishAppliedMove(result);
}

function finishAppliedMove(result) {
  syncMoveHistory();
  renderCurrentBoard(Bridge.getLastCapturedCells());
  updateCaptures();
  renderMoveHistory();
  updateActionButtons();

  if (result === -3) {
    checkGameOver();
    return;
  }
  if (result === -1) {
    setStatus('Draw - board full');
    updateTurnIndicator(0);
    announceGameOver(3);
    updateActionButtons();
    return;
  }
  if (checkGameOver()) return;
  if (result === 2) {
    showBreakableFive();
  } else {
    updateTurnIndicator(Bridge.getCurrentPlayer());
    setStatus('');
  }

  var aiToPlay = currentMode === MODE_AI_V_AI
    || (currentMode === MODE_HUMAN_AI && Bridge.getCurrentPlayer() === 2);
  if (aiToPlay) scheduleAiTurn();
  else maybeRefreshHint();
}
