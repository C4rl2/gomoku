# Gomoku

## Requirements

- Docker
- Python 3

## Run

```bash
make run
```

Builds the WASM binary via Docker and opens the game in your browser at `http://localhost:8080`.

## Other targets

```bash
make cpp      # Recompile C++ → WASM only (server must already be running, then refresh browser)
make fclean   # Remove all build artifacts and Docker image
make help     # List all targets
```
