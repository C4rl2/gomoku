var WIN_MODAL_DELAY_MS = 1650;

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
  var line = Bridge.getWinningLine ? Bridge.getWinningLine() : [];
  if (line && line.length && Render.winningLine)
    Render.winningLine(line);
  updateTurnIndicator(Bridge.getCurrentPlayer());
  setStatus('Breakable five - opponent can still capture');
}

function announceGameOver(winner) {
  var line = (winner === 1 || winner === 2) && Bridge.getWinningLine ? Bridge.getWinningLine() : [];
  if (line && line.length && Render.winningLine) {
    Render.winningLine(line);
    setTimeout(function () {
      showWinModal(winner);
    }, WIN_MODAL_DELAY_MS);
    return;
  }
  showWinModal(winner);
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
