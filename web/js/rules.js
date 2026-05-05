var rulesSlideIndex = 0;
var rulesAnimationTimers = [];
var rulesBoardsRendered = false;
var rulesBoardConfigs = {
  intro: {
    cols: 7,
    rows: 5,
    stones: [
      { x: 1, y: 2, player: 1 },
      { x: 2, y: 2, player: 1 },
      { x: 3, y: 2, player: 1 },
      { x: 4, y: 2, player: 1 },
      { x: 5, y: 2, player: 1 }
    ],
    winning: [
      { x: 1, y: 2 },
      { x: 2, y: 2 },
      { x: 3, y: 2 },
      { x: 4, y: 2 },
      { x: 5, y: 2 }
    ]
  },
  capture: {
    cols: 7,
    rows: 5,
    stones: [
      { x: 1, y: 2, player: 2 },
      { x: 2, y: 2, player: 1 },
      { x: 3, y: 2, player: 1 },
      { x: 4, y: 2, player: 2, target: true }
    ]
  },
  'free-diagonal': {
    cols: 5,
    rows: 5,
    stones: [
      { x: 1, y: 1, player: 1 },
      { x: 2, y: 2, player: 1 },
      { x: 3, y: 3, player: 1 }
    ]
  },
  'free-gap': {
    cols: 6,
    rows: 3,
    stones: [
      { x: 1, y: 1, player: 2 },
      { x: 3, y: 1, player: 2 },
      { x: 4, y: 1, player: 2 }
    ]
  },
  'double-three': {
    cols: 8,
    rows: 5,
    stones: [
      { x: 1, y: 1, player: 2 },
      { x: 2, y: 2, player: 2 },
      { x: 3, y: 4, player: 1, soft: true },
      { x: 4, y: 4, player: 2, target: true },
      { x: 5, y: 4, player: 2 },
      { x: 6, y: 4, player: 2 }
    ]
  },
  endgame: {
    cols: 7,
    rows: 5,
    stones: [
      { x: 2, y: 0, player: 1 },
      { x: 1, y: 2, player: 2 },
      { x: 2, y: 2, player: 2 },
      { x: 3, y: 2, player: 2 },
      { x: 4, y: 2, player: 2 },
      { x: 5, y: 2, player: 2 },
      { x: 4, y: 3, player: 2 },
      { x: 5, y: 4, player: 1, soft: true }
    ],
    winning: [
      { x: 1, y: 2 },
      { x: 2, y: 2 },
      { x: 3, y: 2 },
      { x: 4, y: 2 },
      { x: 5, y: 2 }
    ]
  }
};

function openRulesModal(index) {
  var modal = document.getElementById('rules-modal');
  if (!modal) return;

  if (typeof index === 'number')
    rulesSlideIndex = index;
  else
    rulesSlideIndex = 0;

  modal.hidden = false;
  renderRulesBoards();
  updateRulesModal();
}

function closeRulesModal() {
  var modal = document.getElementById('rules-modal');
  if (!modal) return;
  modal.hidden = true;
  clearRulesBoardAnimations();
}

function nextRulesSlide() {
  var slides = getRuleSlides();
  if (!slides.length) return;
  if (rulesSlideIndex >= slides.length - 1) {
    closeRulesModal();
    return;
  }
  rulesSlideIndex++;
  updateRulesModal();
}

function prevRulesSlide() {
  if (rulesSlideIndex <= 0) return;
  rulesSlideIndex--;
  updateRulesModal();
}

function setRulesSlide(index) {
  rulesSlideIndex = index;
  updateRulesModal();
}

function getRuleSlides() {
  return document.querySelectorAll('[data-rule-slide]');
}

function buildRulesProgress() {
  var progress = document.getElementById('rules-progress');
  var slides = getRuleSlides();
  if (!progress || !slides.length) return;
  if (progress.children.length === slides.length) return;

  progress.innerHTML = '';
  for (var i = 0; i < slides.length; i++) {
    var step = document.createElement('button');
    step.type = 'button';
    step.className = 'rules-step';
    step.setAttribute('aria-label', 'Open rules step ' + (i + 1));
    step.dataset.index = i;
    step.onclick = function () {
      setRulesSlide(parseInt(this.dataset.index, 10));
    };
    progress.appendChild(step);
  }
}

function updateRulesModal() {
  var slides = getRuleSlides();
  var backBtn = document.getElementById('rules-back-btn');
  var nextLabel = document.getElementById('rules-next-label');
  var progress = document.getElementById('rules-progress');
  var steps = document.querySelectorAll('.rules-step');
  if (!slides.length) return;

  if (rulesSlideIndex < 0) rulesSlideIndex = 0;
  if (rulesSlideIndex >= slides.length) rulesSlideIndex = slides.length - 1;

  for (var i = 0; i < slides.length; i++) {
    slides[i].classList.toggle('rules-slide--active', i === rulesSlideIndex);
  }

  for (var s = 0; s < steps.length; s++) {
    steps[s].classList.toggle('rules-step--active', s === rulesSlideIndex);
    steps[s].classList.toggle('rules-step--done', s < rulesSlideIndex);
    steps[s].setAttribute('aria-current', s === rulesSlideIndex ? 'step' : 'false');
  }

  if (backBtn) backBtn.disabled = rulesSlideIndex === 0;
  if (nextLabel)
    nextLabel.textContent = rulesSlideIndex === slides.length - 1 ? "Let's play" : 'Next';
  if (progress) {
    var start = slides.length > 0 ? 100 / (slides.length * 2) : 0;
    var track = 100 - (start * 2);
    var fill = slides.length > 1 ? (rulesSlideIndex / (slides.length - 1) * track) : 0;
    progress.style.setProperty('--rules-progress-start', start + '%');
    progress.style.setProperty('--rules-progress-fill', fill + '%');
  }

  activateRulesBoardAnimations();
}

document.addEventListener('DOMContentLoaded', function () {
  renderRulesBoards();
  buildRulesProgress();
  updateRulesModal();

  document.addEventListener('keydown', function (event) {
    var modal = document.getElementById('rules-modal');
    if (!modal || modal.hidden || event.key !== 'Escape') return;
    closeRulesModal();
  });
});

function renderRulesBoards() {
  var boards = document.querySelectorAll('[data-rules-board]');
  if (!boards.length || rulesBoardsRendered) return;

  for (var i = 0; i < boards.length; i++) {
    renderRulesBoard(boards[i]);
  }
  rulesBoardsRendered = true;
}

function renderRulesBoard(boardEl) {
  var name = boardEl.dataset.rulesBoard;
  var config = rulesBoardConfigs[name];
  if (!config) return;

  boardEl.style.setProperty('--rule-cols', config.cols);
  boardEl.style.setProperty('--rule-rows', config.rows);
  boardEl.innerHTML = '';

  for (var y = 0; y < config.rows; y++) {
    for (var x = 0; x < config.cols; x++) {
      var cell = document.createElement('div');
      cell.className = 'neo-cell rules-demo-cell';
      cell.dataset.x = x;
      cell.dataset.y = y;

      var stoneInfo = findRulesStone(config, x, y);
      if (stoneInfo)
        cell.appendChild(createRulesStone(stoneInfo));

      boardEl.appendChild(cell);
    }
  }
}

function findRulesStone(config, x, y) {
  for (var i = 0; i < config.stones.length; i++) {
    if (config.stones[i].x === x && config.stones[i].y === y)
      return config.stones[i];
  }
  return null;
}

function createRulesStone(stoneInfo) {
  var stone = document.createElement('div');
  stone.className = 'neo-stone ' + (stoneInfo.player === 1 ? 'black' : 'white');
  if (stoneInfo.soft)
    stone.className += ' rules-demo-stone--soft';
  if (stoneInfo.target)
    stone.className += ' rules-demo-stone--target';
  return stone;
}

function activateRulesBoardAnimations() {
  clearRulesBoardAnimations();

  var activeSlide = document.querySelector('.rules-slide--active');
  if (!activeSlide) return;

  var boards = activeSlide.querySelectorAll('[data-rules-board]');
  for (var i = 0; i < boards.length; i++) {
    scheduleRulesWinningLine(boards[i]);
  }
}

function clearRulesBoardAnimations() {
  for (var t = 0; t < rulesAnimationTimers.length; t++)
    clearTimeout(rulesAnimationTimers[t]);
  rulesAnimationTimers = [];

  var highlighted = document.querySelectorAll('.rules-demo-cell.neo-cell--winning');
  for (var i = 0; i < highlighted.length; i++)
    highlighted[i].classList.remove('neo-cell--winning');
}

function scheduleRulesWinningLine(boardEl) {
  var config = rulesBoardConfigs[boardEl.dataset.rulesBoard];
  if (!config || !config.winning) return;

  for (var i = 0; i < config.winning.length; i++) {
    (function (cellInfo, delay) {
      var timer = setTimeout(function () {
        var selector = '.rules-demo-cell[data-x="' + cellInfo.x + '"][data-y="' + cellInfo.y + '"]';
        var cell = boardEl.querySelector(selector);
        if (cell) cell.classList.add('neo-cell--winning');
      }, delay);
      rulesAnimationTimers.push(timer);
    })(config.winning[i], i * 260);
  }
}
