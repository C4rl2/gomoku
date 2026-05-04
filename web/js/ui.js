function updateDebugPanel() {
  var total = Bridge.getLastTTotal();
  var heur = Bridge.getLastTHeuristic();
  var order = Bridge.getLastTMoveOrder();
  var zobrist = Bridge.getLastTZobrist();
  var tt = Bridge.getLastTTT();
  var coreMM = Math.max(0, total - (heur + order + zobrist + tt));

  document.getElementById('dbg-id').textContent = total.toFixed(2);
  document.getElementById('dbg-eval').textContent = heur.toFixed(2);
  document.getElementById('dbg-order').textContent = order.toFixed(2);
  document.getElementById('dbg-zobrist').textContent = zobrist.toFixed(3);
  document.getElementById('dbg-tt').textContent = tt.toFixed(2);
  document.getElementById('dbg-mm').textContent = coreMM.toFixed(2);
  document.getElementById('dbg-cutoffs').textContent = Bridge.getLastCutoffs();
  document.getElementById('dbg-tthits').textContent = Bridge.getLastTTHits();
  document.getElementById('dbg-nodes').textContent = Bridge.getLastNodes();

  updateDebugBar('dbg-mm-bar', coreMM, total);
  updateDebugBar('dbg-eval-bar', heur, total);
  updateDebugBar('dbg-order-bar', order, total);
  updateDebugBar('dbg-zobrist-bar', zobrist, total);
  updateDebugBar('dbg-tt-bar', tt, total);
}

function updateDebugBar(id, value, total) {
  var el = document.getElementById(id);
  if (!el) return;
  var pct = total > 0 ? Math.max(2, Math.min(100, value / total * 100)) : 0;
  el.style.width = pct + '%';
}

function updateCaptures() {
  var capByBlue = Bridge.getCaptures(1);
  var capByPink = Bridge.getCaptures(2);
  document.getElementById('cap-black').textContent = capByBlue;
  document.getElementById('cap-white').textContent = capByPink;
  updateLives(5 - capByPink, 5 - capByBlue);
}

function updateLives(livesBlue, livesPink) {
  renderLives('lives-1', livesBlue);
  renderLives('lives-2', livesPink);
}

function renderLives(id, remaining) {
  var svg = document.getElementById(id);
  if (!svg) return;
  var dots = svg.querySelectorAll('.life');
  for (var i = 0; i < dots.length; i++)
    dots[i].classList.toggle('life--lost', i >= remaining);
}

function setStatus(msg) {
  document.getElementById('status').textContent = msg;
}

function updateAiStats(time, depth) {
  var timeEl = document.getElementById('ai-time');
  var numEl = document.getElementById('ai-depth-num');
  var fillEl = document.getElementById('ai-depth-fill');
  if (timeEl) timeEl.textContent = time.toFixed(3) + 's';
  if (numEl) numEl.textContent = depth + ' / 10';
  if (fillEl) fillEl.style.width = (depth / 10 * 100) + '%';
}

function updateTurnIndicator(player) {
  var card1 = document.getElementById('player-card-1');
  var card2 = document.getElementById('player-card-2');
  if (!card1 || !card2) return;
  card1.classList.toggle('player-card--active', player === 1);
  card2.classList.toggle('player-card--active', player === 2);
}

function updateActionButtons() {
  var undoBtn = document.getElementById('undo-btn');
  var hintBtn = document.getElementById('hint-btn');
  var gameOver = Bridge.isGameOver && Bridge.isGameOver();
  var historyIndex = Bridge.getHistoryIndex ? Bridge.getHistoryIndex() : moveHistory.length;
  if (undoBtn)
    undoBtn.disabled = aiThinking || historyIndex <= 0 || gameOver;
  if (hintBtn)
    hintBtn.disabled = aiThinking || gameOver;
}
