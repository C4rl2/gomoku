document.addEventListener('DOMContentLoaded', function () {
  var themeToggle = document.getElementById('theme-toggle');
  var depthSlider = document.getElementById('depth');
  var savedTheme = localStorage.getItem('gomoku-theme');

  if (savedTheme === 'light')
    document.documentElement.setAttribute('data-theme', 'light');
  else
    document.documentElement.removeAttribute('data-theme');

  if (!themeToggle) return;

  themeToggle.checked = savedTheme === 'light';
  updateDepthDisplay();

  if (depthSlider)
    depthSlider.addEventListener('input', updateDepthDisplay);

  themeToggle.addEventListener('change', function (e) {
    if (e.target.checked) {
      document.documentElement.setAttribute('data-theme', 'light');
      localStorage.setItem('gomoku-theme', 'light');
    } else {
      document.documentElement.removeAttribute('data-theme');
      localStorage.setItem('gomoku-theme', 'dark');
    }

    if (document.getElementById('board') &&
        document.getElementById('game').style.display !== 'none' &&
        Bridge.getBoard) {
      renderCurrentBoard();
    }
  });
});

function updateDepthDisplay() {
  var depthSlider = document.getElementById('depth');
  var depthValue = document.getElementById('depth-value');
  if (!depthSlider || !depthValue) return;
  depthValue.textContent = depthSlider.value;
}
