#include "Game.hpp"
#include <iostream>
#include <string>

Game::Game() {
	this->_currentPlayer = BLACK; //black always starts
	this->_capturesBlack = 0;
	this->_capturesWhite = 0;
}

Game::Game(const Game &other) {
	*this = other;
}

Game &Game::operator=(const Game &other) {
	if (this != &other) {
		this->_board = other._board;
		this->_currentPlayer = other._currentPlayer;
		this->_capturesBlack = other._capturesBlack;
		this->_capturesWhite = other._capturesWhite;
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
	std::cout << "Scores Captures - Noir : " << this->_capturesBlack << " | Blanc : " << this->_capturesWhite << std::endl;
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
			std::cin.ignore(10000, '\n'); //ugly arbitrary number, but will disapear when the game's gonna be playable on the web
			continue;
		}

		if (this->_board.setStone(x, y, this->_currentPlayer)) {
			//detecting captures and incremeting scores
			int captures = this->_board.executeCaptures(x, y, this->_currentPlayer);
			if (captures > 0) {
				if (this->_currentPlayer == BLACK)
					this->_capturesBlack += captures;
				else
					this->_capturesWhite += captures;
			}

			//checking for wins
			bool winByAlignment = this->_board.checkWin(x, y, this->_currentPlayer);
			bool winByCapture = (this->_currentPlayer == BLACK && this->_capturesBlack >= 5) || 
								(this->_currentPlayer == WHITE && this->_capturesWhite >= 5);

			if (winByAlignment || winByCapture) {
				std::cout << std::endl;
				this->_board.printBoard();
				std::string winnerName = (this->_currentPlayer == BLACK) ? "Noir (X)" : "Blanc (O)";
				std::cout << "\n======================================" << std::endl;
				std::cout << " GAGNANT : " << winnerName << " !  " << std::endl;
				std::cout << "======================================" << std::endl;
				break;
			}
			this->_switchPlayer();
		} else {
			std::cout << "Coup invalide ! La case est deja occupee ou hors limites." << std::endl;
		}
	}
}