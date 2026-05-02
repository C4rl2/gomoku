
var Render = (function() {
  var boardEl = null;
  var cells   = [];

  function init(el) {
    boardEl = el;
    boardEl.innerHTML = "";
    cells = [];

    for (var y = 0; y < 19; y++) {
      for (var x = 0; x < 19; x++) {
        var cell = document.createElement("div");
        cell.className = "neo-cell";
        cell.dataset.x = x;
        cell.dataset.y = y;
        boardEl.appendChild(cell);
        cells.push(cell);
      }
    }
  }

  function board(stones) {
    for (var i = 0; i < 19 * 19; i++) {
        var s = stones[i];
        var cell = cells[i];

        if (s === 0) {
            cell.innerHTML = "";
        }
        else {
            var existingStone = cell.querySelector(".neo-stone");
            var colorClass = s === 1 ? "black" : "white";
            if (!existingStone) {
              var stone = document.createElement("div");
              stone.className = "neo-stone " + colorClass;
              cell.appendChild(stone);
            } else {
              existingStone.className = "neo-stone " + colorClass;
            }
        }
    }
  }

  function cellFromClick(evt) {
    var cell = evt.target.closest(".neo-cell");
    if (!cell) return null;
    return {
      x: parseInt(cell.dataset.x, 10),
      y: parseInt(cell.dataset.y, 10)
    };
  }

  return {
    init:          init,
    board:         board,
    cellFromClick: cellFromClick
  };
})();

