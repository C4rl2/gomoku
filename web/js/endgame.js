function checkGameOver() {
  if (!Bridge.isGameOver()) return false;
  var w = Bridge.getWinner();
  var names = { 1: 'Blue', 2: 'Pink', 3: 'Draw' };
  setStatus(w === 3 ? 'Draw!' : names[w] + ' wins!');
  updateTurnIndicator(w === 3 ? 0 : w);
  announceGameOver(w);
  updateActionButtons();
  return true;
}

function showBreakableFive() {
  var lastMove = moveHistory.length > 0 ? moveHistory[moveHistory.length - 1] : null;
  if (!lastMove) return;
  var line = findWinningLine(lastMove.player);
  if (line && Render.winningLine)
    Render.winningLine(line);
  updateTurnIndicator(Bridge.getCurrentPlayer());
  setStatus('Breakable five - opponent can still capture');
}

function announceGameOver(winner) {
  var line = winner === 1 || winner === 2 ? findWinningLine(winner) : null;
  if (line && Render.winningLine) {
    Render.winningLine(line);
    setTimeout(function () {
      showWinModal(winner);
    }, 1650);
    return;
  }
  showWinModal(winner);
}

function findWinningLine(player) {
  var board = Bridge.getBoard();
  var directions = [
    { dx: 1, dy: 0 },
    { dx: 0, dy: 1 },
    { dx: 1, dy: 1 },
    { dx: 1, dy: -1 }
  ];
  var lastMove = moveHistory.length > 0 ? moveHistory[moveHistory.length - 1] : null;
  var fallback = null;

  for (var y = 0; y < 19; y++) {
    for (var x = 0; x < 19; x++) {
      if (board[y * 19 + x] !== player) continue;
      for (var d = 0; d < directions.length; d++) {
        var dx = directions[d].dx;
        var dy = directions[d].dy;
        var prevX = x - dx;
        var prevY = y - dy;
        if (isOnBoard(prevX, prevY) && board[prevY * 19 + prevX] === player) continue;

        var run = [];
        var cx = x;
        var cy = y;
        while (isOnBoard(cx, cy) && board[cy * 19 + cx] === player) {
          run.push({ x: cx, y: cy });
          cx += dx;
          cy += dy;
        }

        if (run.length >= 5) {
          var line = pickFiveFromRun(run, lastMove);
          if (lastMove && lineContains(line, lastMove)) return line;
          if (!fallback) fallback = line;
        }
      }
    }
  }
  return fallback;
}

function pickFiveFromRun(run, preferredMove) {
  if (!preferredMove || !lineContains(run, preferredMove)) return run.slice(0, 5);
  var preferredIndex = -1;
  for (var i = 0; i < run.length; i++) {
    if (run[i].x === preferredMove.x && run[i].y === preferredMove.y) {
      preferredIndex = i;
      break;
    }
  }
  var start = Math.max(0, Math.min(preferredIndex - 2, run.length - 5));
  return run.slice(start, start + 5);
}

function lineContains(line, cell) {
  if (!line || !cell) return false;
  for (var i = 0; i < line.length; i++) {
    if (line[i].x === cell.x && line[i].y === cell.y) return true;
  }
  return false;
}

function isOnBoard(x, y) {
  return x >= 0 && x < 19 && y >= 0 && y < 19;
}

function showWinModal(winner) {
  var modal = document.getElementById('win-modal');
  var title = document.getElementById('win-title');
  var kicker = document.getElementById('win-kicker');
  var message = document.getElementById('win-message');
  if (!modal || !title || !kicker || !message) return;

  modal.classList.remove('win-modal--blue', 'win-modal--pink', 'win-modal--draw');

  if (winner === 1) {
    modal.classList.add('win-modal--blue');
    kicker.textContent = 'Victory';
    title.textContent = 'Blue wins!';
    message.textContent = 'Blue takes the board.';
  } else if (winner === 2) {
    modal.classList.add('win-modal--pink');
    kicker.textContent = 'Victory';
    title.textContent = 'Pink wins!';
    message.textContent = 'Pink closes the game.';
  } else {
    modal.classList.add('win-modal--draw');
    kicker.textContent = 'Game over';
    title.textContent = 'Draw!';
    message.textContent = 'No winner this time.';
  }

  modal.hidden = false;
}

function closeWinModal() {
  var modal = document.getElementById('win-modal');
  if (!modal) return;
  modal.hidden = true;
}
