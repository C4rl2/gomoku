var aiThinking = false;
var currentMode = 0;
var currentSuggestion = null;
var moveHistory = [];
var previewHistoryIndex = null;
var hintsEnabled = false;

var COORD_COLS = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'];

var MODE_HUMAN_AI = 0;
var MODE_HVH = 1;
var MODE_AI_V_AI = 2;
