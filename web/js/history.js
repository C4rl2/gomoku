function syncMoveHistory(limit) {
  if (!Bridge.getMoveHistory) return;
  moveHistory = Bridge.getMoveHistory(limit);
}

function renderCurrentBoard() {
  previewHistoryIndex = null;
  syncMoveHistory();
  if (Render.clearWinHighlights) Render.clearWinHighlights();
  Render.board(Bridge.getBoard(), moveHistory);
  if (currentSuggestion && Render.suggestion && !Bridge.isGameOver())
    Render.suggestion(currentSuggestion);
}

function renderMoveHistory(previewIndex) {
  var list = document.getElementById('move-history');
  var latestBtn = document.getElementById('history-latest-btn');
  if (!list || !Bridge.getHistoryLength || !Bridge.getHistoryIndex) return;

  var length = Bridge.getHistoryLength();
  var currentIndex = Bridge.getHistoryIndex();
  list.innerHTML = '';

  if (latestBtn)
    latestBtn.disabled = aiThinking || currentIndex === length - 1;

  if (length <= 1) {
    var empty = document.createElement('p');
    empty.className = 'history-empty';
    empty.textContent = 'No moves yet';
    list.appendChild(empty);
    return;
  }

  list.appendChild(createHistoryRow(0, currentIndex, previewIndex));
  for (var i = 1; i < length; i++)
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
  if (index > 0) {
    var player = Bridge.getHistoryMovePlayer(index);
    stone.className += player === 1 ? ' history-move__stone--blue' : ' history-move__stone--pink';
  }
  btn.appendChild(stone);

  var coord = document.createElement('span');
  coord.className = 'history-move__coord';
  coord.textContent = index === 0 ? 'Start' : formatMoveCoord(index);
  btn.appendChild(coord);

  return btn;
}

function previewHistory(index) {
  if (aiThinking || !Bridge.getHistoryBoard) return;
  previewHistoryIndex = index;
  syncMoveHistory(index);
  if (Render.clearWinHighlights) Render.clearWinHighlights();
  Render.board(Bridge.getHistoryBoard(index), moveHistory);
  updateHistoryPreviewClass(index);
}

function restoreHistoryPreview() {
  if (previewHistoryIndex === null) return;
  renderCurrentBoard();
  renderMoveHistory();
}

function gotoHistoryIndex(index) {
  if (aiThinking || !Bridge.gotoHistory) return;
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
  if (!Bridge.getHistoryLength) return;
  gotoHistoryIndex(Bridge.getHistoryLength() - 1);
}

function updateUiFromBridgeState() {
  renderCurrentBoard();
  updateCaptures();
  if (Bridge.isGameOver()) {
    var winner = Bridge.getWinner();
    var names = { 1: 'Blue', 2: 'Pink', 3: 'Draw' };
    setStatus(winner === 3 ? 'Draw!' : names[winner] + ' wins!');
    updateTurnIndicator(winner === 3 ? 0 : winner);
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
