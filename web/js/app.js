var aiThinking = false;

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
      Render.board(Bridge.getBoard());
    }
  });
});

Bridge.load(function() {
  document.getElementById('start-btn').disabled = false;
});

function startGame() {
  var depth = Math.max(1, Math.min(10, parseInt(document.getElementById('depth').value) || 5));
  Bridge.gameInit(depth);

  document.getElementById('content').style.display = 'none';
  document.getElementById('setup').style.display = 'none';
  document.getElementById('game').style.display  = '';

  var canvas = document.getElementById('board');
  Render.init(canvas);
  canvas.addEventListener('click', onCanvasClick);

  Render.board(Bridge.getBoard());
  setStatus('Black to play');
}

function onCanvasClick(evt) {
  if (aiThinking || Bridge.isGameOver()) return;
  if (Bridge.getCurrentPlayer() !== 1) return;

  var cell = Render.cellFromClick(evt);
  if (!cell) return;

  var result = Bridge.placeStone(cell.x, cell.y);
  if (result === -1) { setStatus('Invalid move'); return; }
  if (result === -2) { setStatus('Double-three forbidden!'); return; }

  Render.board(Bridge.getBoard());
  updateCaptures();
  if (checkGameOver()) return;

  aiThinking = true;
  setStatus('AI thinking...');
  setTimeout(runAi, 20);
}

function runAi() {
  var result = Bridge.aiPlay();
  var t      = Bridge.getLastAiTime();
  aiThinking = false;

  Render.board(Bridge.getBoard());
  updateCaptures();
  document.getElementById('ai-time').textContent = 'AI time: ' + t.toFixed(3) + 's';

  if (result === -1) { setStatus('Draw — board full'); return; }
  if (checkGameOver()) return;
  setStatus('Black to play');
}

function checkGameOver() {
  if (!Bridge.isGameOver()) return false;
  var w     = Bridge.getWinner();
  var names = { 1: 'Black', 2: 'White', 3: 'Draw' };
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
