var Render = (function() {
  var CELL = 32;
  var PAD  = 24;
  var SIZE = CELL * 18 + PAD * 2;

  var canvas = null;
  var ctx    = null;

  function init(c) {
    canvas        = c;
    canvas.width  = SIZE;
    canvas.height = SIZE;
    ctx           = canvas.getContext('2d');
  }

  function board(stones) {
    ctx.fillStyle = '#dcb06a';
    ctx.fillRect(0, 0, SIZE, SIZE);

    ctx.strokeStyle = '#000';
    ctx.lineWidth   = 0.5;
    for (var i = 0; i < 19; i++) {
      ctx.beginPath();
      ctx.moveTo(PAD + i * CELL, PAD);
      ctx.lineTo(PAD + i * CELL, PAD + 18 * CELL);
      ctx.stroke();
      ctx.beginPath();
      ctx.moveTo(PAD,             PAD + i * CELL);
      ctx.lineTo(PAD + 18 * CELL, PAD + i * CELL);
      ctx.stroke();
    }

    for (var y = 0; y < 19; y++) {
      for (var x = 0; x < 19; x++) {
        var s = stones[y * 19 + x];
        if (s === 0) continue;
        ctx.beginPath();
        ctx.arc(PAD + x * CELL, PAD + y * CELL, CELL * 0.44, 0, Math.PI * 2);
        ctx.fillStyle   = s === 1 ? '#111' : '#eee';
        ctx.fill();
        ctx.strokeStyle = '#555';
        ctx.lineWidth   = 0.5;
        ctx.stroke();
      }
    }
  }

  function cellFromClick(evt) {
    var rect = canvas.getBoundingClientRect();
    var x    = Math.round((evt.clientX - rect.left - PAD) / CELL);
    var y    = Math.round((evt.clientY - rect.top  - PAD) / CELL);
    if (x < 0 || x >= 19 || y < 0 || y >= 19) return null;
    return { x: x, y: y };
  }

  return {
    init:          init,
    board:         board,
    cellFromClick: cellFromClick,
    SIZE:          SIZE
  };
})();
