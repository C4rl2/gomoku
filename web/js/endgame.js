var WIN_MODAL_DELAY_MS = 1650;

function setEndgameStatus() {
  var w = Bridge.getWinner();
  if (w === 3) {
    setStatus('Draw!');
    updateTurnIndicator(0);
  } else {
    setStatus((w === 1 ? 'Blue' : 'Pink') + ' wins!');
    updateTurnIndicator(w);
  }
}

function checkGameOver() {
  if (!Bridge.isGameOver()) return false;
  setEndgameStatus();
  announceGameOver(Bridge.getWinner());
  updateActionButtons();
  return true;
}

function announceGameOver(winner) {
  var line = (winner === 1 || winner === 2) ? Bridge.getWinningLine() : [];
  if (line.length) {
    Render.winningLine(line);
    setTimeout(function () { showWinModal(winner); }, WIN_MODAL_DELAY_MS);
    return;
  }
  showWinModal(winner);
}

function showWinModal(winner) {
  var modal = document.getElementById('win-modal');
  var title = document.getElementById('win-title');
  var kicker = document.getElementById('win-kicker');
  var message = document.getElementById('win-message');

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
  document.getElementById('win-modal').hidden = true;
}
