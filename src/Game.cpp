#include "Game.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <iomanip>

Game::Game() : _ai(WHITE) {
	this->_currentPlayer = BLACK; //black always starts
}

Game::Game(const Game &other) {
	*this = other;
}

Game &Game::operator=(const Game &other) {
	if (this != &other) {
		this->_board = other._board;
		this->_currentPlayer = other._currentPlayer;
		this->_ai = other._ai;
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

	int chosenDepth = 0;
	std::cout << "\nChoisissez la difficulté du joueur IA (profondeur de recherche) : " << std::endl;
	std::cout << "Entrez votre choix (1 à 10) : ";
	while (!(std::cin >> chosenDepth) || chosenDepth < 1 || chosenDepth > 10) {
		if (std::cin.eof()) {
			std::cout << "Arret du programme." << std::endl;
			return ;
		}
		std::cout << "Entrée invalide. Veuillez entrer un nombre entre 1 et 10 : ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	this->_ai.setDepth(chosenDepth);
	std::cout << "Difficultée reglée sur la profondeur " << this->_ai.getDepth() << ".\n" << std::endl;

	while (true) {
		std::cout << std::endl;
		this->_board.printBoard();
		this->_printTurnInfo();
		
		if (this->_currentPlayer == BLACK) {
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

		} else {
			std::cout << "L'IA réfléchit..." << std::endl;
			clock_t start_time = clock(); //starting timer
			Move aiMove = this->_ai.getBestMove(this->_board);
			x = aiMove.x;
			y = aiMove.y; //ai move got calculated
			
			if (x == -1 && y == -1) {
				std::cout << "\n======================================" << std::endl;
				std::cout << " MATCH NUL ! Le plateau est complet. " << std::endl;
				std::cout << "======================================" << std::endl;
				break;
			}

			clock_t end_time = clock(); //stoppped the timer
			
			double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
			std::cout << ">> Coup joué par l'IA : " << x << " " << y << std::endl;
			std::cout << ">> Temps de calcul : " << std::fixed << std::setprecision(6) << time_spent << " secondes." << std::endl;
			std::cout.unsetf(std::ios_base::floatfield); //remove the precision for secondes
			std::cout << "Profondeur : " << this->_ai.getDepth() << std::endl;
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