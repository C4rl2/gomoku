#include "Board.hpp"

int main() {
	Board board;

	std::cout << "Placement d'une pierre noire au centre (9, 9)..." << std::endl;
	board.setStone(9, 9, BLACK);

	std::cout << "Placement d'une pierre blanche à côté (10, 9)..." << std::endl;
	board.setStone(10, 9, WHITE);

	std::cout << "\nÉtat du plateau :" << std::endl;
	board.printBoard();

	return 0;
}