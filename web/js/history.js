function syncMoveHistory(limit) {
  moveHistory = Bridge.getMoveHistory(limit);
}

function renderCurrentBoard(captureCells) {
  previewHistoryIndex = null;
  syncMoveHistory();
  Render.clearWinHighlights();
  Render.board(Bridge.getBoard(), moveHistory, { captureCells: captureCells || [] });
  if (currentSuggestion && !Bridge.isGameOver())
    Render.suggestion(currentSuggestion);
}

function renderMoveHistory(previewIndex) {
  var list = document.getElementById('move-history');
  var latestBtn = document.getElementById('history-latest-btn');
  if (!list) return;

  var length = Bridge.getHistoryLength();
  var currentIndex = Bridge.getHistoryIndex();
  list.innerHTML = '';

  latestBtn.disabled = aiThinking || currentIndex === length - 1;

  if (length <= 1) {
    var empty = document.createElement('p');
    empty.className = 'history-empty';
    empty.textContent = 'No moves yet';
    list.appendChild(empty);
    return;
  }

  for (var i = 0; i < length; i++)
    list.appendChild(createHistoryRow(i, currentIndex, previewIndex));
}

function createHistoryRow(index, currentIndex, previewIndex) {
  var btn = document.createElement('button');
  btn.type = 'button';
  btn.className = 'history-move';
  btn.dataset.historyIndex = index;
  if (index === currentIndex) btn.className += ' history-move--active';
  if (index === previewIndex) btn.className += ' history-move--preview';
  btn.onmouseenter = function () { previewHistory(index); };
  btn.onclick = function () { gotoHistoryIndex(index); };

  var num = document.createElement('span');
  num.className = 'history-move__num';
  num.textContent = index === 0 ? '00' : padMoveNumber(index);
  btn.appendChild(num);

  var stone = document.createElement('span');
  stone.className = 'history-move__stone';
  var player = 0;
  if (index > 0) {
    player = Bridge.getHistoryMovePlayer(index);
    stone.className += player === 1 ? ' history-move__stone--blue' : ' history-move__stone--pink';
  }
  btn.appendChild(stone);

  var coord = document.createElement('span');
  coord.className = 'history-move__coord';
  coord.textContent = index === 0 ? 'Start' : formatMoveCoord(index);
  btn.appendChild(coord);

  btn.appendChild(createHistoryBadges(index, player));

  return btn;
}

function createHistoryBadges(index, player) {
  var wrap = document.createElement('span');
  wrap.className = 'history-move__badges';
  if (index <= 0 || !player) return wrap;

  var board = BoardUtils.snapshot(Bridge.getHistoryBoard(index));
  var x = Bridge.getHistoryMoveX(index);
  var y = Bridge.getHistoryMoveY(index);
  var captures = Bridge.getHistoryCapturedCount(index);
  var run = BoardUtils.longestRunThrough(board, x, y, player);

  if (captures > 0)
    wrap.appendChild(createHistoryBadge('capture', 'x' + captures));
  if (run >= 5)
    wrap.appendChild(createHistoryBadge('threat', '5'));
  else if (run >= 4)
    wrap.appendChild(createHistoryBadge('threat', '4'));
  return wrap;
}

function createHistoryBadge(type, label) {
  var badge = document.createElement('span');
  badge.className = 'history-badge history-badge--' + type;
  badge.textContent = label;
  return badge;
}

function previewHistory(index) {
  if (aiThinking) return;
  previewHistoryIndex = index;
  syncMoveHistory(index);
  Render.clearWinHighlights();
  Render.board(Bridge.getHistoryBoard(index), moveHistory);
  updateHistoryPreviewClass(index);
}

function restoreHistoryPreview() {
  if (previewHistoryIndex === null) return;
  renderCurrentBoard();
  renderMoveHistory();
}

function gotoHistoryIndex(index) {
  if (aiThinking) return;
  closeWinModal();
  if (Bridge.gotoHistory(index) !== 0) return;
  currentSuggestion = null;
  updateUiFromBridgeState();
  maybeRefreshHint();
}

function updateHistoryPreviewClass(index) {
  var rows = document.querySelectorAll('.history-move');
  for (var i = 0; i < rows.length; i++) {
    rows[i].classList.toggle(
      'history-move--preview',
      parseInt(rows[i].dataset.historyIndex, 10) === index
    );
  }
}

function gotoLatestHistory() {
  gotoHistoryIndex(Bridge.getHistoryLength() - 1);
}

function updateUiFromBridgeState() {
  renderCurrentBoard();
  updateCaptures();
  if (Bridge.isGameOver()) {
    setEndgameStatus();
  } else {
    setStatus('');
    updateTurnIndicator(Bridge.getCurrentPlayer());
  }
  updateActionButtons();
  renderMoveHistory();
}

function padMoveNumber(index) {
  return index < 10 ? '0' + index : String(index);
}

function formatMoveCoord(index) {
  var x = Bridge.getHistoryMoveX(index);
  var y = Bridge.getHistoryMoveY(index);
  if (x < 0 || y < 0) return '-';
  return COORD_COLS[x] + (y + 1);
}
