NAME     = Gomoku
PORT     = 8080
IMG      = gomoku-builder

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I $(SRCS_DIR)

EMCC     = em++
EMFLAGS  = -std=c++98 -I $(SRCS_DIR) -O2               \
           -s WASM=1                                    \
           -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
           -s ALLOW_MEMORY_GROWTH=1                     \
           -s MODULARIZE=1                              \
           -s EXPORT_NAME="GomokuModule"                \
           --bind

SRCS_DIR = src
OBJS_DIR = objs
WEB_DIR  = web

SRCS_FILES = main.cpp Board.cpp Game.cpp AI.cpp

SRCS      = $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJS      = $(addprefix $(OBJS_DIR)/, $(SRCS_FILES:.cpp=.o))
WASM_SRCS = $(SRCS)

DOCKER = docker run --rm --platform linux/amd64 \
         -v $(shell pwd):/src -w /src $(IMG)


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@echo "\033[32m[ OK ] $(NAME) built\033[0m"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $@


.docker-stamp: Dockerfile
	docker build --platform linux/amd64 -t $(IMG) .
	@touch $@

$(WEB_DIR)/gomoku.js: $(WASM_SRCS) .docker-stamp
	mkdir -p $(WEB_DIR)
	$(DOCKER) $(EMCC) $(EMFLAGS) $(WASM_SRCS) -o $(WEB_DIR)/gomoku.js
	@echo "\033[32m[ OK ] WASM build done → $(WEB_DIR)/gomoku.js\033[0m"

cpp: $(WEB_DIR)/gomoku.js


run: cpp
	@lsof -ti:$(PORT) | xargs kill -9 2>/dev/null || true
	@echo "\033[32m[Gomoku] http://localhost:$(PORT) — Ctrl+C to stop\033[0m"
	@(sleep 0.3 && (open http://localhost:$(PORT) || xdg-open http://localhost:$(PORT)) 2>/dev/null) &
	@python3 -m http.server $(PORT) --directory $(WEB_DIR)


clean:
	rm -rf $(OBJS_DIR)
	@echo "\033[33m[ clean ] Object files removed\033[0m"

fclean: clean
	rm -f $(NAME)
	rm -f $(WEB_DIR)/gomoku.js $(WEB_DIR)/gomoku.wasm
	rm -f .docker-stamp
	@lsof -ti:$(PORT) | xargs kill -9 2>/dev/null || true
	docker ps -aq --filter ancestor=$(IMG) | xargs docker rm -f 2>/dev/null || true
	docker rmi $(IMG) 2>/dev/null || true
	docker images -f dangling=true -q | xargs docker rmi 2>/dev/null || true
	@echo "\033[33m[ fclean ] Binary, WASM, server and Docker images removed\033[0m"

re: fclean all

help:
	@echo ""
	@echo "  \033[1mGomoku — available targets\033[0m"
	@echo ""
	@echo "  \033[32mmake\033[0m          Build the native terminal binary (./$(NAME))"
	@echo "  \033[32mmake cpp\033[0m      Recompile C++ → WASM (incremental, server must already be running)"
	@echo "  \033[32mmake run\033[0m      Build WASM + launch game in browser"
	@echo "  \033[32mmake clean\033[0m    Remove object files"
	@echo "  \033[32mmake fclean\033[0m   Remove objects, binary, WASM and Docker images"
	@echo "  \033[32mmake re\033[0m       fclean + all (native rebuild)"
	@echo "  \033[32mmake help\033[0m     Show this message"
	@echo ""

.PHONY: all cpp run clean fclean re help
