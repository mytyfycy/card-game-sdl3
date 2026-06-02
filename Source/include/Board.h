#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "TextRenderer.h"
#include "TextureManager.h"

class Board {
public:
	Board(SDL_Renderer* renderer, const char* fontPath);
	void render(const GameState& state);
	void onCardHovered(int index) { m_hoveredCard = index; }
	void onSnatchHovered(int index) { m_snatchHoveredCard = index; }

	void clearTextureCache() { m_text.clearCache(); }
	void clearTextCache() { m_textures.clearCache(); }

	TextRenderer* getTextRenderer() { return &m_text; }
	TextureManager* getTextureManager() { return &m_textures; }

private:
	int m_hoveredCard = -1;
	int m_snatchHoveredCard = -1;
	SDL_Renderer* m_renderer;
	TextRenderer m_text;
	TextureManager m_textures;
	
	void drawDivider();
	void drawDeckStack(float x, float y, bool inverted = false);
	void drawHandStack(float x, float y, int count, int hoveredIdx);
	void drawFieldCards(float x, float y, const std::vector<Card>& cards, bool inverted = false);
	void drawHandCards(float x, float y, const std::vector<Card>& cards, int hoveredIdx);
	void drawScorePanel(int playerScore, int oppScore);
	void drawGameOver(GameResult result);
	void drawLastPlayed(const GameState& state);

	void drawCard(float x, float y, float w, float h, const Card& card, bool inverted = false);

	void drawTexture(float x, float y, float w, float h, const Card& card, bool inverted = false);

	void drawSnatchPrompt();

	std::string cardNameOf(const Card& card) const;
};