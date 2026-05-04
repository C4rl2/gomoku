# Gomoku

An implementation of the **Gomoku** board game playable in the browser, featuring a custom AI opponent written from scratch in C++ and compiled to WebAssembly.

## What is Gomoku?

Gomoku ("five in a row") is a two-player strategy game played on a 19×19 grid. Each player places stones in turn; the first to align five stones horizontally, vertically, or diagonally wins.

This version implements the **Pro / Renju-inspired ruleset** used in the 42 school project:

- **Captures** : flanking exactly two adjacent enemy stones with your own captures and removes them from the board.
- **Capture win** : capturing 10 enemy stones (5 pairs) wins the game.
- **Double-three rule** : a move that simultaneously creates two free three-in-a-rows is forbidden, *unless* that move also performs a capture.
- **Breakable five** : an alignment of five can still be broken on the next turn if one of its stones can be captured. The game only ends once a five-in-a-row survives the opponent's reply.

<img width="1919" height="905" alt="Web Page Screenshot" src="https://github.com/user-attachments/assets/970a005e-48b7-4191-ba8a-91d4dbdb830d" />

## The project in one picture

```
┌──────────────────────┐    WASM    ┌──────────────────────┐
│  C++ engine (C++98)  │ ─────────► │  Browser (vanilla JS)│
│  Board / Game / AI   │  bridge.js │  Canvas rendering    │
│  Minimax + α-β + TT  │            │  Click → place stone │
└──────────────────────┘            └──────────────────────┘
```

Three layers, one game loop:

1. A **C++ engine** owns the rules and the AI search.
2. **Emscripten** compiles it to WebAssembly inside a reproducible Docker build.
3. A **vanilla JS / Canvas frontend** renders the board and forwards user input through a thin `ccall` bridge.

## Tech stack

| Layer        | Technology                                                                 |
|--------------|----------------------------------------------------------------------------|
| Game engine  | **C++98**, compiled with `-Wall -Wextra -Werror`                          |
| AI           | Hand-rolled **minimax** with α-β pruning, iterative deepening, transposition table, Zobrist hashing |
| Compilation  | **Emscripten** (`emscripten/emsdk:3.1.74`) targeting **WebAssembly**       |
| Build env    | **Docker** (`linux/amd64`) — fully reproducible, no host toolchain needed |
| Frontend     | **Vanilla JavaScript**, HTML5 **Canvas**, no framework, no bundler        |
| Server       | Python 3 built-in HTTP server                                              |
| Build system | **Makefile** orchestrating Docker, Emscripten, and the dev server          |

## How the AI works

<img width="688" height="283" alt="minimax-alpha-beta" src="https://github.com/user-attachments/assets/1d850b90-da6b-4079-9575-e0f6b7829435" />

The opponent is a classic **minimax search with alpha-beta pruning**, hardened with several techniques to make it strong within a strict per-move time budget (500 ms):

- **Iterative deepening** — the search restarts at depth 1, 2, 3… and the best move from the previous depth seeds the next one. Whenever the time budget runs out mid-search, the last completed depth is used.
- **Transposition table** — a fixed-size hash table (1,048,576 entries, power-of-two for fast masking) caches previously evaluated positions with `EXACT` / `LOWER` / `UPPER` bound flags.
- **Zobrist hashing** — each `(x, y, color)` triple has a random 64-bit key, XORed in/out as stones are placed and captured. Captures update the hash incrementally to mirror the engine's actual capture logic exactly.
- **Move ordering** — candidate moves are scored by a fast heuristic (alignments, captures, threats) and explored best-first to maximize α-β cutoffs.
- **Heuristic evaluation** — a positional score combining open/closed twos, threes, fours, captured stones, and threat patterns.

The UI surfaces both the **AI's thinking time** and the **actual depth reached** at each move, so you can see iterative deepening at work.

## Project structure

```
src/                  C++ engine
├──Board.{hpp,cpp}     Grid, rules, captures, win detection
├──Game.{hpp,cpp}      State machine driving one move at a time
├──AI.{hpp,cpp}        Minimax + α-β + TT + Zobrist
└──main.cpp            Emscripten extern "C" exports
web/
├──index.html
└──js/
    ├──bridge.js      ccall wrappers around the WASM exports
    ├──render.js      Canvas drawing
    └──app.js         Event flow, click → place_stone → ai_play
Dockerfile            Pinned emsdk image for reproducible builds
Makefile              Build, run, clean targets
```

## Try it

### Requirements

You only need two tools on your machine — everything else (the C++ toolchain, Emscripten, etc.) is fetched and run inside Docker.

- **Docker**  ![Docker](https://img.shields.io/badge/docker-%230db7ed.svg?style=flat&logo=docker&logoColor=white)
- **Python 3**  ![Python](https://img.shields.io/badge/Python-3776AB?style=flat&logo=python&logoColor=white)

### Run

```bash
git clone <this-repo-url>
cd gomoku
make run
```

This will:

1. Build the `gomoku-builder` Docker image (first run only).
2. Compile the C++ engine to `web/gomoku.js` + `web/gomoku.wasm` via Emscripten.
3. Start a local HTTP server on port 8080.
4. Open your browser at [http://localhost:8080](http://localhost:8080).

Choose an AI search depth (1–10), click an intersection to place a black stone, and the AI replies as white.

### Other Make targets

```bash
make cpp      # Incremental WASM rebuild (server already running → just refresh browser)
make fclean   # Remove all build artifacts, the Docker image, and free port 8080
make help     # List all targets
```

## Constraints worth knowing about

This is a **42 school** project, which imposes a few non-obvious constraints I had to design around:

- **C++98 only** — no `auto`, no range-for, no `nullptr`, no `<chrono>`, no smart pointers. All timing is done through `clock()` from `<ctime>`.
- **`-Werror` everywhere** — every warning is a build failure.
- The AI must reply **in under 500 ms per move** on a 19×19 board, which is what motivated the iterative-deepening + transposition-table + Zobrist hashing stack rather than a simpler fixed-depth search.
