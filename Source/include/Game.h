#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "Board.h"

class Game {
public:
	Game(SDL_Renderer* renderer);
	void handleEvent(const SDL_Event& e);
	void update();
	void render();

private:
	GameState m_state;
	Board m_board;

	int m_hoveredCard = -1;
	int m_selectedCard = -1;

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
	void checkRoundEnd();

	int cardHitTest(float mx, float my) const; // indeks karty w rece ktory kliknieto
};