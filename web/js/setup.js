function applyTheme(theme) {
  if (theme === 'dark')
    document.documentElement.removeAttribute('data-theme');
  else
    document.documentElement.setAttribute('data-theme', 'light');
}

document.addEventListener('DOMContentLoaded', function () {
  var themeToggle = document.getElementById('theme-toggle');
  var depthSlider = document.getElementById('depth');
  var savedTheme = localStorage.getItem('gomoku-theme') === 'dark' ? 'dark' : 'light';

  applyTheme(savedTheme);
  themeToggle.checked = savedTheme === 'dark';
  updateDepthDisplay();

  depthSlider.addEventListener('input', updateDepthDisplay);

  themeToggle.addEventListener('change', function (e) {
    var theme = e.target.checked ? 'dark' : 'light';
    applyTheme(theme);
    localStorage.setItem('gomoku-theme', theme);
    if (document.getElementById('game').style.display !== 'none')
      renderCurrentBoard();
  });
});

function updateDepthDisplay() {
  document.getElementById('depth-value').textContent = document.getElementById('depth').value;
}
