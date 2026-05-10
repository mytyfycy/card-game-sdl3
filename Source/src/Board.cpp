#include "Board.h"
#include <SDL3/SDL.h>

using namespace Layout;

Board::Board(SDL_Renderer* renderer, const char* fontPath) 
	: m_renderer(renderer), 
	m_text(renderer, fontPath),
	m_textures(renderer) {}

void Board::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}

void Board::fillRect(float x, float y, float w, float h) {
	SDL_FRect rc{ x, y, w, h };
	SDL_RenderFillRect(m_renderer, &rc);
}

void Board::drawRect(float x, float y, float w, float h) {
	SDL_FRect rc{ x, y, w, h };
	SDL_RenderRect(m_renderer, &rc);
}

void Board::drawBackground() {
	setColor(26, 74, 46);
	fillRect(0, 0, WIN_W, WIN_H);
}

void Board::drawDivider() {
	setColor(45, 110, 69);
	for (float x = 60.f; x < WIN_W - 60.f; x += 28.f) {
		SDL_FRect seg{ x, DIVIDER_Y - 1.f, 16.f, 2.f };
		SDL_RenderFillRect(m_renderer, &seg);
	}
	for (float x = 60.f; x < WIN_W - 60.f; x += 28.f) {
		SDL_FRect seg{ x, DIVIDER_Y/2 - 1.f, 16.f, 2.f };
		SDL_RenderFillRect(m_renderer, &seg);
	}
	for (float x = 60.f; x < WIN_W - 60.f; x += 28.f) {
		SDL_FRect seg{ x, DIVIDER_Y * 1.5 - 1.f, 16.f, 2.f };
		SDL_RenderFillRect(m_renderer, &seg);
	}
}

void Board::drawDeckStack(float x, float y, bool inverted) {
	Card back;
	for (int i = 2; i >= 0; --i) {
		float ox = x + i * 6.f;
		float oy = y - i * 6.f;
		if (inverted)
			drawCard(ox, oy, CARD_W, CARD_H, back, true);
		else
			drawCard(ox, oy, CARD_W, CARD_H, back);
	}
}

void Board::drawHandStack(float x, float y, int count) {
	Card back;
	for (int i = 0; i < count; ++i) {
		float cx = x + i * (HAND_CARD_W + HAND_GAP);
		drawCard(cx, y, HAND_CARD_W, HAND_CARD_H, back, true);
	}
}

void Board::drawTexture(float x, float y, float w, float h, const Card& card, bool inverted) {
	float angle = inverted ? 180.f : 0.f;
	SDL_FPoint center = { w / 2.f, h / 2.f };

	if (!card.texturePath.empty()) {
		SDL_Texture* texture = m_textures.get(card.texturePath);
		if (texture) {
			SDL_FRect dst{ x,y,w,h };
			SDL_RenderTextureRotated(m_renderer, texture, nullptr, &dst, angle, &center, SDL_FLIP_NONE);
		}
	}
}

void Board::drawCard(float x, float y, float w, float h, const Card& card, bool inverted) {
	// Rewers
	if (card.type == CardType::Back) {
		setColor(138, 26, 42);
		fillRect(x, y, w, h);
		setColor(192, 64, 96);
		drawRect(x, y, w, h);

		setColor(192, 64, 96, 100);
		SDL_RenderLine(m_renderer, x + 10, y + 10, x + w - 10, y + h - 10);
		SDL_RenderLine(m_renderer, x + w - 10, y + 10, x + 10, y + h - 10);

		drawTexture(x, y, w, h, card, inverted);

		return;
	}

	// Awers
	switch (card.type) {
	case CardType::Number:
		setColor(232, 221, 208); break;
	case CardType::Strike:
		setColor(200, 212, 240); break;
	case CardType::Flip:
		setColor(224, 212, 240); break;
	case CardType::Blast:
		setColor(240, 220, 200); break;
	case CardType::Force:
		setColor(232, 200, 248); break;
	}

	fillRect(x, y, w, h);

	// Tekstura
	drawTexture(x, y, w, h, card, inverted);

	// Obramowanie
	switch (card.type) {
	case CardType::Number: setColor(176, 144, 112); break;
	case CardType::Strike: setColor(112, 144, 208); break;
	case CardType::Flip: setColor(144, 112, 192); break;
	case CardType::Blast: setColor(208, 144, 80); break;
	case CardType::Force: setColor(192, 96, 224); break;
	}

	drawRect(x, y, w, h);

	SDL_Color darkBrown = { 90, 62, 27, 255 };
	SDL_Color effectBlue = { 42, 58, 122, 255 };
	SDL_Color effectPurp = { 58, 26, 122, 255 };

	if (card.type == CardType::Number) {
		// Duza cyfra (srodek)
		std::string val = std::to_string(card.value);
		m_text.draw(val,
			x + w / 2.f, y + h / 2.f,
			static_cast<int>(h * 0.45f),
			darkBrown,
			TextAlign::Center);

		// Mala cyfra (lewy gorny rog)
		m_text.draw(val,
			x + w * 0.15f, y + h * 0.15f,
			static_cast<int>(h * 0.18f),
			darkBrown,
			TextAlign::Center);
	}
	else {
		// Nazwa efektu
		std::string label;
		switch (card.type) {
			case CardType::Strike: label = "STRIKE"; break;
			case CardType::Flip: label = "FLIP"; break;
			case CardType::Blast: label = "BLAST"; break;
			case CardType::Force: label = "FORCE"; break;
			default: break;
		}

		if (!label.empty()) {
			SDL_Color col = (card.type == CardType::Flip || card.type == CardType::Force)
				? effectPurp : effectBlue;

			m_text.draw(label,
				x + w / 2.f, y + h * 0.25f,
				static_cast<int>(h * 0.14f),
				col,
				TextAlign::Center);
		}
	}
}

void Board::drawFieldCards(float x, float y, const std::vector<Card>& cards, bool inverted) {
	if (inverted)
	{
		for (size_t i = 0; i < cards.size(); ++i) {
			float cx = x - i * (CARD_W + CARD_GAP);
			drawCard(cx, y, CARD_W, CARD_H, cards[i], true);
		}
		return;
	}

	for (size_t i = 0; i < cards.size(); ++i) {
		float cx = x + i * (CARD_W + CARD_GAP);
		drawCard(cx, y, CARD_W, CARD_H, cards[i]);
	}
}

void Board::drawGlow(float x, float y, float w, float h) {
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	int layers = 3;
	for (int i = layers; i >= 1; --i) {
		float expand = i * 6.f;
		float alpha = 40.f + (layers - i) * 15.f;

		setColor(255, 220, 80, static_cast<uint8_t>(alpha));
		fillRect(x - expand, y - expand, w + expand * 2, h + expand * 2);
	}

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
}

void Board::drawHandCards(float x, float y, const std::vector<Card>& cards, int hoveredIdx) {
	for (size_t i = 0; i < cards.size(); ++i) {
		if (i == hoveredIdx) continue;
		float cx = x + i * (HAND_CARD_W + HAND_GAP);

		if (i == hoveredIdx)
			drawGlow(cx, y, HAND_CARD_W, HAND_CARD_H);

		drawCard(cx, y, HAND_CARD_W, HAND_CARD_H, cards[i]);
	}

	if (hoveredIdx >= 0 && hoveredIdx < cards.size()) {
		float cx = x + hoveredIdx * (HAND_CARD_W + HAND_GAP);
		float cy = y - 20.f;
		drawGlow(cx, cy, HAND_CARD_W, HAND_CARD_H);
		drawCard(cx, cy, HAND_CARD_W, HAND_CARD_H, cards[hoveredIdx]);
	}
}

void Board::drawScorePanel(int playerScore, int oppScore) {
	using namespace Layout;

	setColor(10, 31, 18, 220);
	fillRect(SCORE_X, SCORE_Y, SCORE_W, SCORE_H);
	setColor(45, 110, 69, 180);
	drawRect(SCORE_X, SCORE_Y, SCORE_W, SCORE_H);

	float cx = SCORE_X + SCORE_W / 2.f;
	SDL_Color scoreCol = { 200, 240, 216, 255 };
	SDL_Color vsCol = { 106,184,128,255 };

	// Wynik przeciwnika (gora)
	m_text.draw(std::to_string(oppScore),
		cx, SCORE_Y + SCORE_H * 0.28f,
		static_cast<int>(SCORE_H * 0.28f),
		scoreCol, TextAlign::Center);

	// VS (srodek)
	m_text.draw("VS",
		cx, SCORE_Y + SCORE_H * 0.52f,
		static_cast<int>(SCORE_H * 0.18f),
		vsCol, TextAlign::Center);

	// Wynik gracza (dol)
	m_text.draw(std::to_string(playerScore),
		cx, SCORE_Y + SCORE_H * 0.76f,
		static_cast<int>(SCORE_H * 0.28f),
		scoreCol, TextAlign::Center);
}

void Board::drawGameOver(GameResult result) {
	using namespace Layout;

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	setColor(0, 0, 0, 160);
	fillRect(0, 0, WIN_W, WIN_H);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

	std::string text = (result == GameResult::PlayerWin) ? "Wygrales" : "Przegrales";

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color yellow = { 255, 210, 80, 255 };

	SDL_Color col = (result == GameResult::PlayerWin) ? yellow : white;

	m_text.draw(text, WIN_W / 2.f, WIN_H / 2.f, 120, col, TextAlign::Center);
	m_text.draw("R - Nowa gra", WIN_W / 2.f, WIN_H / 2.f + 140.f, 40, white, TextAlign::Center);
}

void Board::render(const GameState& state) 
{
	drawBackground();
	drawDivider();

	// Talie
	drawDeckStack(PLY_DECK_X, PLY_DECK_Y);
	drawDeckStack(OPP_DECK_X, OPP_DECK_Y, true);

	// Reka przeciwnika
	drawHandStack(OPP_HAND_X, OPP_HAND_Y, static_cast<int>(state.opponent.hand.size()));

	// Pola gry
	drawFieldCards(OPP_FIELD_X, OPP_FIELD_Y, state.opponent.field, true);
	drawFieldCards(PLY_FIELD_X, PLY_FIELD_Y, state.player.field);

	int hovered = (state.phase == GamePhase::PlayerTurn) ? m_hoveredCard : -1;

	// Reka gracza
	drawHandCards(PLY_HAND_X, PLY_HAND_Y, state.player.hand, hovered);

	drawScorePanel(state.player.score, state.opponent.score);

	if (state.phase == GamePhase::GameOver)
		drawGameOver(state.result);
}