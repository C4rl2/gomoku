#include "Game.hpp"
#include <iostream>
#include <string>

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
	std::string playerName = (this->_currentPlayer == BLACK) ? "Noir (X)" : "Blanc (O)";
	std::cout << "\nTour du joueur " << playerName << "." << std::endl;
	std::cout << "Entrez les coordonnees X et Y (ex: 9 9) pour poser votre pierre ou CTRL+D pour quitter : ";
}

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
			std::cin.ignore(10000, '\n'); 
			continue;
		}

		if (this->_board.setStone(x, y, this->_currentPlayer)) {
			this->_switchPlayer();
		} else {
			std::cout << "Coup invalide ! La case est deja occupee ou hors limites." << std::endl;
		}
	}
}