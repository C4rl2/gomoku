#ifndef AI_HPP
# define AI_HPP

# include "Board.hpp"
# include <limits> //alpha-beta

struct Move {
	int x;
	int y;
};

class AI {
	private:
		e_stone _aiTeam;
		e_stone _opponentTeam;

		// Fonction d'évaluation (L'heuristique)
		// Elle analyse le plateau et donne une note
		int     _evaluateBoard(const Board &board) const;
		
		// Sous-fonction pour analyser une ligne spécifique (comme on a fait pour les Free-Three)
		int     _evaluateLine(int count, int openEnds, bool isAi) const;
	public:
		AI();
		AI(e_stone aiTeam); // Constructeur utile pour choisir la couleur de l'IA
		AI(const AI &other);
		AI &operator=(const AI &other);
		~AI();

		// La fonction publique que Game.cpp va appeler
		// On passe x et y par référence pour récupérer les coordonnées du coup choisi
		Move	getBestMove(const Board &board);
};

#endif