#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "Board.h"
#include "Subscribable.h"
#include "Difficulty.h"

class Game : public Subscribable {
public:
	Game(SDL_Renderer* renderer, Difficulty difficulty);
	void handleEvent(const SDL_Event& e);
	void update();
	void render();
	Board& getBoard() { return m_board; }

private:
	GameState m_state;
	Board m_board;

	int m_hoveredCard = -1;
	int m_snatchHoveredCard = -1;
	int m_selectedCard = -1;
	GamePhase m_snatchCallerTurn = GamePhase::PlayerTurn;
	Difficulty m_difficulty;
	float m_aiBestMoveChance; // cache

	uint64_t m_aiMoveTime = 0;
	static constexpr uint64_t AI_DELAY_MS = 1000;

	void initRound();
	void clearField();
	std::vector<Card> buildDeck();
	void dealOpeningCards();

	void playerPlayCard(int handIndex);
	void aiTakeTurn();
	int aiChooseCard();
	void sortDeck(std::vector<Card>& deck);

	int calcFieldScore(const std::vector<Card>& field) const;
	bool canSurpass(const PlayerState& attacker, int defenderScore) const;
	void applyCard(PlayerState& attacker, PlayerState& defender, int handIndex);
	void handleSnatchSelection(int cardIndex);
	void checkRoundEnd();

	int cardHitTest(float mx, float my) const; // indeks karty w rece ktory kliknieto
	int snatchHitTest(float mx, float my) const;
};