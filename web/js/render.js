
var Render = (function() {
  var boardEl = null;
  var cells   = [];
  var COL_LABELS = ['A','B','C','D','E','F','G','H','J','K','L','M','N','O','P','Q','R','S','T'];

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

    var colsEl = document.getElementById("coords-cols");
    var rowsEl = document.getElementById("coords-rows");
    if (colsEl) {
      colsEl.innerHTML = "";
      for (var c = 0; c < 19; c++) {
        var span = document.createElement("span");
        span.textContent = COL_LABELS[c];
        colsEl.appendChild(span);
      }
    }
    if (rowsEl) {
      rowsEl.innerHTML = "";
      for (var r = 1; r <= 19; r++) {
        var span = document.createElement("span");
        span.textContent = r;
        rowsEl.appendChild(span);
      }
    }
  }

  function board(stones, moveHistory) {
    var history = moveHistory || [];
    for (var i = 0; i < 19 * 19; i++) {
        var s = stones[i];
        var cell = cells[i];

        if (s === 0) {
            cell.innerHTML = "";
        }
        else {
            var existingStone = cell.querySelector(".neo-stone");
            var colorClass = s === 1 ? "black" : "white";
            var moveNum = 0;
            for (var m = 0; m < history.length; m++) {
              if (history[m].y * 19 + history[m].x === i) { moveNum = m + 1; break; }
            }
            if (!existingStone) {
              var stone = document.createElement("div");
              stone.className = "neo-stone " + colorClass;
              if (moveNum > 0) {
                var numSpan = document.createElement("span");
                numSpan.className = "stone-num";
                numSpan.textContent = moveNum;
                stone.appendChild(numSpan);
              }
              cell.appendChild(stone);
            } else {
              existingStone.className = "neo-stone " + colorClass;
              var existingNum = existingStone.querySelector(".stone-num");
              if (moveNum > 0) {
                if (!existingNum) {
                  var numSpan = document.createElement("span");
                  numSpan.className = "stone-num";
                  existingStone.appendChild(numSpan);
                  existingNum = numSpan;
                }
                existingNum.textContent = moveNum;
              } else if (existingNum) {
                existingStone.removeChild(existingNum);
              }
            }
        }
    }
  }

  function clearWinHighlights() {
    if (!boardEl) return;
    var highlighted = boardEl.querySelectorAll(".neo-cell--winning");
    for (var i = 0; i < highlighted.length; i++) {
      highlighted[i].classList.remove("neo-cell--winning");
    }
  }

  function winningLine(line) {
    clearWinHighlights();
    if (!line || !cells.length) return;
    for (var i = 0; i < line.length; i++) {
      (function(cellInfo, delay) {
        setTimeout(function() {
          var idx = cellInfo.y * 19 + cellInfo.x;
          if (cells[idx]) cells[idx].classList.add("neo-cell--winning");
        }, delay);
      })(line[i], i * 260);
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
    marker.style.border         = "2px dashed var(--ui-accent)";
    marker.style.boxShadow      = "0 0 12px var(--ui-accent-muted)";
    marker.style.boxSizing      = "border-box";
    marker.style.pointerEvents  = "none";
    target.style.position = target.style.position || "relative";
    target.appendChild(marker);
  }

  return {
    init:          init,
    board:         board,
    cellFromClick: cellFromClick,
    suggestion:    suggestion,
    winningLine:   winningLine,
    clearWinHighlights: clearWinHighlights
  };
})();
