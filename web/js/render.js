
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

  //draws a translucent hollow marker on the suggested empty cell
  //must be re-applied after every board() call since board() resets innerHTML
  function suggestion(cell) {
    var prev = boardEl ? boardEl.querySelector(".neo-suggestion") : null;
    if (prev && prev.parentNode) prev.parentNode.removeChild(prev);
    if (!cell || !cells.length) return;
    var idx = cell.y * 19 + cell.x;
    var target = cells[idx];
    if (!target) return;
    var marker = document.createElement("div");
    marker.className = "neo-suggestion";
    marker.style.position       = "absolute";
    marker.style.top            = "50%";
    marker.style.left           = "50%";
    marker.style.transform      = "translate(-50%, -50%)";
    marker.style.width          = "60%";
    marker.style.height         = "60%";
    marker.style.borderRadius   = "50%";
    marker.style.border         = "2px dashed rgba(255, 90, 90, 0.85)";
    marker.style.boxSizing      = "border-box";
    marker.style.pointerEvents  = "none";
    target.style.position = target.style.position || "relative";
    target.appendChild(marker);
  }

  return {
    init:          init,
    board:         board,
    cellFromClick: cellFromClick,
    suggestion:    suggestion
  };
})();

