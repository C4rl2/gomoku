var BoardUtils = (function () {
  var BOARD_SIZE = 19;
  var DIRECTIONS = [
    { dx: 1, dy: 0 },
    { dx: 0, dy: 1 },
    { dx: 1, dy: 1 },
    { dx: 1, dy: -1 }
  ];

  function index(x, y) {
    return y * BOARD_SIZE + x;
  }

  function isOnBoard(x, y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
  }

  function snapshot(typedBoard) {
    if (!typedBoard) return [];
    return Array.prototype.slice.call(typedBoard);
  }

  function longestRunThrough(board, x, y, player) {
    if (!board || !isOnBoard(x, y) || !player) return 0;
    var best = 0;
    for (var i = 0; i < DIRECTIONS.length; i++) {
      var dir = DIRECTIONS[i];
      var run = 1 +
        countRun(board, x, y, dir.dx, dir.dy, player) +
        countRun(board, x, y, -dir.dx, -dir.dy, player);
      if (run > best) best = run;
    }
    return best;
  }

  function countRun(board, x, y, dx, dy, player) {
    var count = 0;
    var cx = x + dx;
    var cy = y + dy;
    while (isOnBoard(cx, cy) && board[index(cx, cy)] === player) {
      count++;
      cx += dx;
      cy += dy;
    }
    return count;
  }

  return {
    BOARD_SIZE: BOARD_SIZE,
    index: index,
    isOnBoard: isOnBoard,
    snapshot: snapshot,
    longestRunThrough: longestRunThrough
  };
})();
