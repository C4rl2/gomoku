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
  var pct = total > 0 ? Math.max(2, Math.min(100, value / total * 100)) : 0;
  document.getElementById(id).style.width = pct + '%';
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
  var dots = document.getElementById(id).querySelectorAll('.life');
  for (var i = 0; i < dots.length; i++)
    dots[i].classList.toggle('life--lost', i >= remaining);
}

function setStatus(msg) {
  document.getElementById('status').textContent = msg;
}

function updateAiStats(time, depth) {
  document.getElementById('ai-time').textContent = time.toFixed(3) + 's';
  document.getElementById('ai-depth-num').textContent = depth + ' / 10';
  document.getElementById('ai-depth-fill').style.width = (depth / 10 * 100) + '%';
}

function updateTurnIndicator(player) {
  document.getElementById('player-card-1').classList.toggle('player-card--active', player === 1);
  document.getElementById('player-card-2').classList.toggle('player-card--active', player === 2);
}

function updateActionButtons() {
  var gameOver = Bridge.isGameOver();
  var historyIndex = Bridge.getHistoryIndex();
  document.getElementById('undo-btn').disabled = aiThinking || historyIndex <= 0 || gameOver;
  document.getElementById('hint-btn').disabled = aiThinking || gameOver;
}
