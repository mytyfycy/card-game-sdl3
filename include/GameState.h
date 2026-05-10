#pragma once
#include <vector>
#include "Card.h"

enum class GamePhase {
	Setup,
	PlayerTurn,
	OpponentTurn,
	RoundEnd, // Remis - czyszczenie
	GameOver
};

enum class GameResult { None, PlayerWin, PlayerLose };

struct PlayerState {
	std::vector<Card> hand; // karty w rece
	std::vector<Card> field; // zagrane karty
	int score = 0; // suma wartosci na polu
};

struct GameState {
	PlayerState player;
	PlayerState opponent;
	GamePhase phase = GamePhase::Setup;
	GameResult result = GameResult::None;
	int round = 1;
};