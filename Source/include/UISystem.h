#pragma once
#include "GameEvents.h"

class Game;
class Board;

class UISystem {
public:
	UISystem(Board& board);
	void bindEvents(Game& game);

private:
	Board& m_board;
};