#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "TextRenderer.h"

namespace Layout {
	constexpr float WIN_W = 1920.f;
	constexpr float WIN_H = 1080.f;
	constexpr float DIVIDER_Y = WIN_H / 2.f;

	constexpr float CARD_W = 156.f;
	constexpr float CARD_H = 220.f;
	constexpr float CARD_GAP = 16.f;

	constexpr float PLY_DECK_X = 90.f;
	constexpr float PLY_DECK_Y = 718.f;

	constexpr float OPP_DECK_X = PLY_DECK_X;
	constexpr float OPP_DECK_Y = 230.f;

	constexpr float PLY_HAND_X = 505.f;
	constexpr float PLY_HAND_Y = 883.f;

	constexpr float HAND_CARD_W = 116.f;
	constexpr float HAND_CARD_H = 162.f;
	constexpr float HAND_GAP = 14.f;

	constexpr float OPP_HAND_X = PLY_HAND_X;
	constexpr float OPP_HAND_Y = 36.f;

	constexpr float PLY_FIELD_X = 300.f;
	constexpr float PLY_FIELD_Y = 614.f;
	constexpr float OPP_FIELD_X = 300.f;
	constexpr float OPP_FIELD_Y = 245.f;

	constexpr float SCORE_X = 1500.f;
	constexpr float SCORE_Y = 415.f;
	constexpr float SCORE_W = 278.f;
	constexpr float SCORE_H = 230.f;
}

class Board {
public:
	Board(SDL_Renderer* renderer, const char* fontPath);
	void render(const GameState& state);

private:
	SDL_Renderer* m_renderer;
	TextRenderer m_text;
	
	void drawBackground();
	void drawDivider();
	void drawDeckStack(float x, float y);
	void drawHandStack(float x, float y, int count);
	void drawFieldCards(float x, float y, const std::vector<Card>& cards);
	void drawHandCards(float x, float y, const std::vector<Card>& cards);
	void drawScorePanel(int playerScore, int oppScore);
	void drawGameOver(GameResult result);

	void drawCard(float x, float y, float w, float h, const Card* card);

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void fillRect(float x, float y, float w, float h);
	void drawRect(float x, float y, float w, float h);
};