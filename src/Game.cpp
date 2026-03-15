#include "Game.hpp"
#include <iostream>
#include <string>
#include <limits>

Game::Game() {
	this->_currentPlayer = BLACK; //black always starts
}

Game::Game(const Game &other) {
	*this = other;
}

Game &Game::operator=(const Game &other) {
	if (this != &other) {
		this->_board = other._board;
		this->_currentPlayer = other._currentPlayer;
	}
	return *this;
}

Game::~Game() {}

void Game::_switchPlayer() {
	if (this->_currentPlayer == BLACK) {
		this->_currentPlayer = WHITE;
	} else {
		this->_currentPlayer = BLACK;
	}
}

void Game::_printTurnInfo() const {
	std::cout << "Scores Captures - Noir : " << this->_board.getCaptures(BLACK) << " | Blanc : " << this->_board.getCaptures(WHITE) << std::endl;
	std::string playerName = (this->_currentPlayer == BLACK) ? "Noir (X)" : "Blanc (O)";
	std::cout << "\nTour du joueur " << playerName << "." << std::endl;
	std::cout << "Entrez les coordonnees X et Y (ex: 9 9) pour poser votre pierre ou CTRL+D pour quitter : ";
}

//start the game logic
void Game::run() {
	int x, y;

	std::cout << "=== Gomoku (Mode Terminal) ===" << std::endl;

	while (true) {
		std::cout << std::endl;
		this->_board.printBoard();
		this->_printTurnInfo();
		
		if (!(std::cin >> x >> y)) {
			if (std::cin.eof()) {
				std::cout << "\nArret du programe." << std::endl;
				break;
			}
			std::cout << "Entree invalide. Veuillez entrer deux nombres entre 0 et 18." << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		if (x < 0 || x >= 19 || y < 0 || y >= 19 || this->_board.getStone(x, y) != EMPTY) {
			std::cout << "Coup invalide ! La case est deja occupee ou hors limites." << std::endl;
			continue;
		}

		if (this->_board.isDoubleThree(x, y, this->_currentPlayer) &&
			this->_board.willCapture(x, y, this->_currentPlayer) == false) {
			std::cout << "Coup interdit ! Double trois détecté." << std::endl;
			continue;
		}

		if (this->_board.setStone(x, y, this->_currentPlayer)) {
			//detecting captures and incremeting scores
			this->_board.executeCaptures(x, y, this->_currentPlayer);

			//five stones aligned didn't got break
			e_stone opponent = (this->_currentPlayer == BLACK) ? WHITE : BLACK;
			if (this->_board.hasFive(opponent)) {
				std::cout << std::endl;
				this->_board.printBoard();
				std::string winnerName = (opponent == BLACK) ? "Noir (X)" : "Blanc (O)";
				std::cout << "\n======================================" << std::endl;
				std::cout << "  VICTOIRE DU JOUEUR " << winnerName << " !  " << std::endl;
				std::cout << "  (L'adversaire n'a pas cassé l'alignement)" << std::endl;
				std::cout << "======================================" << std::endl;
				break;
			}

			
			e_win_state winState = this->_board.checkWin(x, y, this->_currentPlayer);
			bool winByCapture = (this->_currentPlayer == BLACK && this->_board.getCaptures(BLACK) >= 5) ||
								(this->_currentPlayer == WHITE && this->_board.getCaptures(WHITE) >= 5);

			if (winState == WIN || winByCapture) {
				std::cout << std::endl;
				this->_board.printBoard();
				std::string winnerName = (this->_currentPlayer == BLACK) ? "Noir (X)" : "Blanc (O)";
				std::cout << "\n======================================" << std::endl;
				std::cout << " GAGNANT : " << winnerName << " !  " << std::endl;
				std::cout << "======================================" << std::endl;
				break;
			} else if (winState == BREAKABLE_FIVE) {
				std::cout << "\n[!] ALERTE : Alignement de 5 cassable crée !" << std::endl;
			}

			this->_switchPlayer();
		} else {
			std::cout << "Coup invalide ! La case est deja occupee ou hors limites." << std::endl;
		}
	}
}