#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "TextRenderer.h"
#include "TextureManager.h"

namespace Layout {
	constexpr float WIN_W = 1920.f;
	constexpr float WIN_H = 1080.f;
	constexpr float DIVIDER_Y = WIN_H * 0.5f;

	constexpr float CARD_W = 156.f;
	constexpr float CARD_H = 220.f;
	constexpr float CARD_GAP = -96.f;

	constexpr float PLY_DECK_X = 300.f;
	constexpr float PLY_DECK_Y = 845.f;

	constexpr float OPP_DECK_X = 1320.f;
	constexpr float OPP_DECK_Y = 30.f;

	constexpr float PLY_HAND_X = 600.f;
	constexpr float PLY_HAND_Y = 883.f;

	constexpr float HAND_CARD_W = 116.f;
	constexpr float HAND_CARD_H = 162.f;
	constexpr float HAND_GAP = -32.f;

	constexpr float OPP_HAND_X = PLY_HAND_X - 300.f;
	constexpr float OPP_HAND_Y = 36.f;

	constexpr float PLY_FIELD_X = WIN_W * 0.5 + CARD_W - 65.f;
	constexpr float PLY_FIELD_Y = 565.f;
	constexpr float OPP_FIELD_X = WIN_W * 0.5 - CARD_W - (65.f * 1.25);
	constexpr float OPP_FIELD_Y = 295.f;

	constexpr float SCORE_X = WIN_W * 0.5 - 65.f;
	constexpr float SCORE_Y = 415.f;
	constexpr float SCORE_W = 139.f;
	constexpr float SCORE_H = 230.f;
}

class Board {
public:
	Board(SDL_Renderer* renderer, const char* fontPath);
	void render(const GameState& state);

private:
	SDL_Renderer* m_renderer;
	TextRenderer m_text;
	TextureManager m_textures;
	
	void drawBackground();
	void drawDivider();
	void drawDeckStack(float x, float y, bool inverted = false);
	void drawHandStack(float x, float y, int count);
	void drawFieldCards(float x, float y, const std::vector<Card>& cards, bool inverted = false);
	void drawHandCards(float x, float y, const std::vector<Card>& cards);
	void drawScorePanel(int playerScore, int oppScore);
	void drawGameOver(GameResult result);

	void drawCard(float x, float y, float w, float h, const Card& card, bool inverted = false);

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void fillRect(float x, float y, float w, float h);
	void drawRect(float x, float y, float w, float h);

	void drawTexture(float x, float y, float w, float h, const Card& card, bool inverted = false);
};