#include "Board.h"
#include <SDL3/SDL.h>
#include "Layout.h"

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
	fillRect(0, 0, Layout::WIN_W, Layout::WIN_H);
}

void Board::drawDivider() {
	setColor(45, 110, 69);

	float startX = Layout::scF(60.f);
	float endX = Layout::WIN_W - startX;
	float step = Layout::scF(28.f);
	float segW = Layout::scF(16.f);
	float segH = Layout::scF(2.f);
	float halfSegH = segH * 0.5f;

	for (float x = startX; x < endX; x += step) {
		SDL_FRect seg{ x, Layout::DIVIDER_Y() - halfSegH, segW, segH };
		SDL_RenderFillRect(m_renderer, &seg);
	}

	for (float x = startX; x < endX; x += step) {
		SDL_FRect seg{ x, (Layout::DIVIDER_Y() / 2.f) - halfSegH, segW, segH };
		SDL_RenderFillRect(m_renderer, &seg);
	}

	for (float x = startX; x < endX; x += step) {
		SDL_FRect seg{ x, (Layout::DIVIDER_Y() * 1.5f) - halfSegH, segW, segH };
		SDL_RenderFillRect(m_renderer, &seg);
	}
}

void Board::drawDeckStack(float x, float y, bool inverted) {
	Card back;
	for (int i = 2; i >= 0; --i) {
		float ox = x + i * 6.f;
		float oy = y - i * 6.f;
		if (inverted)
			drawCard(ox, oy, Layout::CARD_W(), Layout::CARD_H(), back, true);
		else
			drawCard(ox, oy, Layout::CARD_W(), Layout::CARD_H(), back);
	}
}

void Board::drawHandStack(float x, float y, int count, int hoveredIdx) {
	Card back;
	for (int i = 0; i < count; ++i) {
		if (i == hoveredIdx) continue;
		float cx = x + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), back, true);
	}

	if (hoveredIdx >= 0 && hoveredIdx < count) {
		float cx = x + hoveredIdx * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		drawGlow(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H());
		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), back, true);
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
	case CardType::Snatch:
		setColor(240, 220, 200); break;
	case CardType::Double:
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
	case CardType::Snatch: setColor(208, 144, 80); break;
	case CardType::Double: setColor(192, 96, 224); break;
	}

	drawRect(x, y, w, h);

	SDL_Color darkBrown = { 90, 62, 27, 255 };
	SDL_Color effectBlue = { 42, 58, 122, 255 };
	SDL_Color effectPurp = { 58, 26, 122, 255 };

	//if (card.type == CardType::Number) {
	//	// Duza cyfra (srodek)
	//	std::string val = std::to_string(card.value);
	//	m_text.draw(val,
	//		x + w / 2.f, y + h / 2.f,
	//		static_cast<int>(h * 0.45f),
	//		darkBrown,
	//		TextAlign::Center);

	//	// Mala cyfra (lewy gorny rog)
	//	m_text.draw(val,
	//		x + w * 0.15f, y + h * 0.15f,
	//		static_cast<int>(h * 0.18f),
	//		darkBrown,
	//		TextAlign::Center);
	//}
	//else {
	//	// Nazwa efektu
	//	std::string label;
	//	switch (card.type) {
	//		case CardType::Strike: label = "STRIKE"; break;
	//		case CardType::Flip: label = "FLIP"; break;
	//		case CardType::Snatch: label = "SNATCH"; break;
	//		case CardType::Double: label = "DOUBLE"; break;
	//		default: break;
	//	}

	//	if (!label.empty()) {
	//		SDL_Color col = (card.type == CardType::Flip || card.type == CardType::Double)
	//			? effectPurp : effectBlue;

	//		m_text.draw(label,
	//			x + w / 2.f, y + h * 0.25f,
	//			static_cast<int>(h * 0.14f),
	//			col,
	//			TextAlign::Center);
	//	}
	//}
}

void Board::drawFieldCards(float x, float y, const std::vector<Card>& cards, bool inverted) {
	if (inverted)
	{
		for (size_t i = 0; i < cards.size(); ++i) {
			float cx = x - i * (Layout::CARD_W() + Layout::CARD_GAP());
			drawCard(cx, y, Layout::CARD_W(), Layout::CARD_H(), cards[i], true);
		}
		return;
	}

	for (size_t i = 0; i < cards.size(); ++i) {
		float cx = x + i * (Layout::CARD_W() + Layout::CARD_GAP());
		drawCard(cx, y, Layout::CARD_W(), Layout::CARD_H(), cards[i]);
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
		float cx = x + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());

		if (i == hoveredIdx)
			drawGlow(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H());

		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), cards[i]);
	}

	if (hoveredIdx >= 0 && hoveredIdx < cards.size()) {
		float cx = x + hoveredIdx * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		float cy = y - 20.f;
		drawGlow(cx, cy, Layout::HAND_CARD_W(), Layout::HAND_CARD_H());
		drawCard(cx, cy, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), cards[hoveredIdx]);
	}
}

void Board::drawScorePanel(int playerScore, int oppScore) {

	float sx = Layout::SCORE_X();
	float sy = Layout::SCORE_Y();
	float sw = Layout::SCORE_W();
	float sh = Layout::SCORE_H();

	setColor(10, 31, 18, 220);
	fillRect(sx, sy, sw, sh);

	setColor(45, 110, 69, 180);
	drawRect(sx, sy, sw, sh);

	float cx = sx + sw * 0.5f;

	SDL_Color scoreCol = { 200, 240, 216, 255 };
	SDL_Color vsCol = { 106,184,128,255 };

	int scoreFontSize = Layout::scFont(52.f);
	int vsFontSize = Layout::scFont(42.f);

	// Wynik przeciwnika (gora)
	m_text.draw(std::to_string(oppScore),
		cx, sy + sh * 0.24f,
		scoreFontSize,
		scoreCol, TextAlign::Center);

	// VS (srodek)
	m_text.draw("VS",
		cx, sy + sh * 0.52f,
		vsFontSize,
		vsCol, TextAlign::Center);

	// Wynik gracza (dol)
	m_text.draw(std::to_string(playerScore),
		cx, sy + sh * 0.8f,
		scoreFontSize,
		scoreCol, TextAlign::Center);
}

void Board::drawGameOver(GameResult result) {

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	setColor(0, 0, 0, 160);
	fillRect(0, 0, Layout::WIN_W, Layout::WIN_H);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

	std::string text = (result == GameResult::PlayerWin) ? "Wygrales" : "Przegrales";

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color yellow = { 255, 210, 80, 255 };

	SDL_Color col = (result == GameResult::PlayerWin) ? yellow : white;

	float centerX = Layout::WIN_W * 0.5f;
	float centerY = Layout::WIN_H * 0.5f;

	m_text.draw(text, centerX, centerY - Layout::scF(40.f), Layout::scFont(120.f), col, TextAlign::Center);

	float subtitleY = centerY + Layout::scF(40.f);
	m_text.draw("R - Nowa gra", centerX, subtitleY, Layout::scFont(40.f), white, TextAlign::Center);
}

void Board::drawLastPlayed(const GameState& state) {
	SDL_Color dim = { 163, 217, 184, 255 };
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color red = { 255, 100, 100, 255 };

	float x = Layout::fromRight(1920.f - 20.f);
	float y = Layout::fromTop(20.f);

	if (state.phase == GamePhase::PlayerTurn && state.lastPlayedCard.has_value() && !state.lastPlayedByPlayer) {
		m_text.draw("Last Played:", x, y, Layout::scFont(28.f), dim, TextAlign::Right);
		y += Layout::scF(40.f);

		m_text.draw(cardNameOf(state.lastPlayedCard.value()), x, y, Layout::scFont(42), white, TextAlign::Right);
		y += Layout::scF(60.f);
	}

	if (state.lastSnatchedCard.has_value()) {
		m_text.draw("Snatched:", x, y, Layout::scFont(28.f), dim, TextAlign::Right);
		y += Layout::scF(40.f);
		m_text.draw(cardNameOf(state.lastSnatchedCard.value()), x, y, Layout::scFont(42.f), red, TextAlign::Right);
	}

	if (state.lastRestoredCard.has_value()) {
		SDL_Color green = { 100, 255, 150, 255 };
		m_text.draw("Restored:", x, y, 28, dim, TextAlign::Right);
		y += Layout::scFont(40.f);

		m_text.draw(cardNameOf(state.lastRestoredCard.value()), x, y, Layout::scFont(42.f), green, TextAlign::Right);
		y += Layout::scFont(60.f);
	}
}

void Board::render(const GameState& state) 
{
	drawBackground();
	drawDivider();

	// Talie
	drawDeckStack(Layout::PLY_DECK_X(), Layout::PLY_DECK_Y());
	drawDeckStack(Layout::OPP_DECK_X(), Layout::OPP_DECK_Y(), true);

	// Reka przeciwnika - hover podczas Snatch
	int oppHovered = (state.phase == GamePhase::SelectingSnatchTarget ? m_snatchHoveredCard : -1);

	// Reka przeciwnika
	drawHandStack(Layout::OPP_HAND_X(), Layout::OPP_HAND_Y(), static_cast<int>(state.opponent.hand.size()), oppHovered);

	// Pola gry
	drawFieldCards(Layout::OPP_FIELD_X(), Layout::OPP_FIELD_Y(), state.opponent.field, true);
	drawFieldCards(Layout::PLY_FIELD_X(), Layout::PLY_FIELD_Y(), state.player.field);

	int plyHovered = (state.phase == GamePhase::PlayerTurn) ? m_hoveredCard : -1;

	// Reka gracza
	drawHandCards(Layout::PLY_HAND_X(), Layout::PLY_HAND_Y(), state.player.hand, plyHovered);

	drawScorePanel(state.player.score, state.opponent.score);

	if (state.phase == GamePhase::PlayerTurn)
		drawLastPlayed(state);

	if (state.phase == GamePhase::SelectingSnatchTarget)
		drawSnatchPrompt();

	if (state.phase == GamePhase::GameOver)
		drawGameOver(state.result);
}

void Board::drawSnatchPrompt() {
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	setColor(0, 0, 0, 120);

	float rectY = Layout::OPP_HAND_Y() - Layout::scF(40.f);
	float rectH = Layout::HAND_CARD_H() - Layout::scF(80.f);

	fillRect(0, rectY, Layout::WIN_W, rectH);

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

	float textX = Layout::WIN_W * 0.5f;
	float textY = Layout::OPP_HAND_Y();

	int fontSize = Layout::scFont(36.f);

	SDL_Color white = { 255, 255, 255, 255 };
	m_text.draw("Wybierz karte przeciwnika, ktora chcesz usunac", 
		textX, textY, fontSize, white, TextAlign::Center);
}

std::string Board::cardNameOf(const Card& card) const {
	switch (card.type) {
	case CardType::Number: return std::to_string(card.value);
	case CardType::Strike: return "Strike";
	case CardType::Flip: return "Flip";
	case CardType::Snatch: return "Snatch";
	case CardType::Double: return "Double";
	default: return "?";
	}
}