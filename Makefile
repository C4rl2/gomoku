NAME        = gomoku

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -I $(SRCS_DIR)

#defining the OS for the graphic part
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    #add graphic libs here
    OS_FLAGS = 
endif
ifeq ($(UNAME_S),Darwin)
    #add graphic libs here
    OS_FLAGS = 
endif

CXXFLAGS += $(OS_FLAGS)

SRCS_DIR    = src
OBJS_DIR    = objs


SRCS_FILES  = main.cpp \
              Board.cpp \
			  Game.cpp \
			  AI.cpp

SRCS        = $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJS        = $(addprefix $(OBJS_DIR)/, $(SRCS_FILES:.cpp=.o))


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[32m[ OK ] | L'exécutable $(NAME) a été créé avec succès !\033[0m"


$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)
	@echo "\033[33m[ Nettoyage ] | Fichiers objets supprimés.\033[0m"

fclean: clean
	rm -f $(NAME)
	@echo "\033[33m[ Nettoyage ] | Exécutable $(NAME) supprimé.\033[0m"

re: fclean all

.PHONY: all clean fclean re