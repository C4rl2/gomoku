var Render = (function () {
  var CAPTURE_ANIMATION_MS = 560;
  var WIN_STONE_DELAY_MS = 260;
  var SIZE = BoardUtils.BOARD_SIZE;
  var boardEl = null;
  var cells = [];

  function init(el) {
    boardEl = el;
    boardEl.innerHTML = "";
    cells = [];

    for (var y = 0; y < SIZE; y++) {
      for (var x = 0; x < SIZE; x++) {
        var cell = document.createElement("div");
        cell.className = "neo-cell";
        cell.dataset.x = x;
        cell.dataset.y = y;
        boardEl.appendChild(cell);
        cells.push(cell);
      }
    }

    fillCoords("coords-cols", function (i) { return COORD_COLS[i]; });
    fillCoords("coords-rows", function (i) { return i + 1; });
  }

  function fillCoords(id, label) {
    var el = document.getElementById(id);
    if (!el) return;
    el.innerHTML = "";
    for (var i = 0; i < SIZE; i++) {
      var span = document.createElement("span");
      span.textContent = label(i);
      el.appendChild(span);
    }
  }

  function board(stones, moveHistory, options) {
    var moveNumbers = buildMoveNumberMap(moveHistory || []);
    var captureMap = buildCaptureMap(options && options.captureCells);
    for (var i = 0; i < SIZE * SIZE; i++) {
      var s = stones[i];
      var cell = cells[i];

      if (s === 0) {
        if (captureMap[i] && cell.querySelector(".neo-stone")) {
          animateCapturedCell(cell);
        } else if (!cell.classList.contains("neo-cell--capturing")) {
          cell.innerHTML = "";
        }
        continue;
      }

      var existingStone = cell.querySelector(".neo-stone");
      var colorClass = s === 1 ? "black" : "white";
      var moveNum = moveNumbers[i] || 0;
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
        cell.classList.remove("neo-cell--capturing");
        existingStone.className = "neo-stone " + colorClass;
        var existingNum = existingStone.querySelector(".stone-num");
        if (moveNum > 0) {
          if (!existingNum) {
            existingNum = document.createElement("span");
            existingNum.className = "stone-num";
            existingStone.appendChild(existingNum);
          }
          existingNum.textContent = moveNum;
        } else if (existingNum) {
          existingStone.removeChild(existingNum);
        }
      }
    }
  }

  function buildCaptureMap(captureCells) {
    var map = {};
    if (!captureCells) return map;
    for (var i = 0; i < captureCells.length; i++)
      map[BoardUtils.index(captureCells[i].x, captureCells[i].y)] = true;
    return map;
  }

  function buildMoveNumberMap(history) {
    var map = {};
    for (var i = 0; i < history.length; i++)
      map[BoardUtils.index(history[i].x, history[i].y)] = i + 1;
    return map;
  }

  function animateCapturedCell(cell) {
    var stone = cell.querySelector(".neo-stone");
    if (!stone) return;
    cell.classList.add("neo-cell--capturing");
    stone.classList.add("neo-stone--captured");
    setTimeout(function () {
      if (!cell.classList.contains("neo-cell--capturing")) return;
      cell.innerHTML = "";
      cell.classList.remove("neo-cell--capturing");
    }, CAPTURE_ANIMATION_MS);
  }

  function clearWinHighlights() {
    if (!boardEl) return;
    var highlighted = boardEl.querySelectorAll(".neo-cell--winning");
    for (var i = 0; i < highlighted.length; i++)
      highlighted[i].classList.remove("neo-cell--winning");
  }

  function winningLine(line) {
    clearWinHighlights();
    if (!line || !cells.length) return;
    for (var i = 0; i < line.length; i++) {
      (function (cellInfo, delay) {
        setTimeout(function () {
          var idx = BoardUtils.index(cellInfo.x, cellInfo.y);
          if (cells[idx]) cells[idx].classList.add("neo-cell--winning");
        }, delay);
      })(line[i], i * WIN_STONE_DELAY_MS);
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

  // Re-applied after board() because empty cells are rebuilt.
  function suggestion(cell) {
    var prev = boardEl ? boardEl.querySelector(".neo-suggestion") : null;
    if (prev && prev.parentNode) prev.parentNode.removeChild(prev);
    if (!cell || !cells.length) return;
    var target = cells[BoardUtils.index(cell.x, cell.y)];
    if (!target) return;
    var marker = document.createElement("div");
    marker.className = "neo-suggestion";
    var core = document.createElement("span");
    core.className = "neo-suggestion__core";
    marker.appendChild(core);
    target.appendChild(marker);
  }

  return {
    init: init,
    board: board,
    cellFromClick: cellFromClick,
    suggestion: suggestion,
    winningLine: winningLine,
    clearWinHighlights: clearWinHighlights
  };
})();
