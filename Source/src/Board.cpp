/*!
	\file Board.cpp
	\brief Implementation of Board

	Contains all SDL draw calls for the game view: card rendering,
	layout helpers, score panel, overlays, and hover effects.
*/

#include "Board.h"
#include <SDL3/SDL.h>
#include "Layout.h"
#include "DrawUtil.h"

/*!
	Registers the TextureManager with the draw utility on construction
	so that \c draw::drawTexture() can resolve texture paths immediately.
*/
Board::Board(SDL_Renderer* renderer, const char* fontPath) 
	: m_renderer(renderer), 
	m_text(renderer, fontPath),
	m_textures(renderer) {
	draw::setTextureManager(&m_textures);
}

//! Draws three rows of dashed segments at the divider, mid-top, and mid-bottom positions.
void Board::drawDivider() {
	draw::setColor(45, 110, 69);

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

/*!
	Draws three offset copies of a face-down card to simulate a stack.
	Each copy is shifted slightly right and up relative to the previous.
*/
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

/*!
	Skips the hovered card in the main pass, then redraws it last
	so it renders on top of all others with a glow highlight.
*/
void Board::drawHandStack(float x, float y, int count, int hoveredIdx) {
	Card back;
	for (int i = 0; i < count; ++i) {
		if (i == hoveredIdx) continue;
		float cx = x + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), back, true);
	}

	if (hoveredIdx >= 0 && hoveredIdx < count) {
		float cx = x + hoveredIdx * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		draw::drawGlow(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H());
		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), back, true);
	}
}

//! Delegates to \c draw::drawTexture() with a 180 degree rotation when \a inverted is \c true.
void Board::drawTexture(float x, float y, float w, float h, const Card& card, bool inverted) {
	float angle = inverted ? 180.f : 0.f;
	SDL_FPoint center = { w / 2.f, h / 2.f };

	if (!card.texturePath.empty()) {
		draw::drawTexture(card.texturePath, x, y, w, h, angle, &center);
	}
}

/*!
	Draws the card back (CardType::Back) as a solid rectangle with cross lines.
	For all other types, fills with a type-specific background color,
	overlays the card texture, then draws a type-colored border.
*/
void Board::drawCard(float x, float y, float w, float h, const Card& card, bool inverted) {
	if (card.type == CardType::Back) {
		draw::setColor(138, 26, 42);
		draw::fillRect(x, y, w, h);
		draw::setColor(192, 64, 96);
		draw::drawRect(x, y, w, h);

		draw::setColor(192, 64, 96, 100);
		SDL_RenderLine(m_renderer, x + 10, y + 10, x + w - 10, y + h - 10);
		SDL_RenderLine(m_renderer, x + w - 10, y + 10, x + 10, y + h - 10);

		drawTexture(x, y, w, h, card, inverted);

		return;
	}

	switch (card.type) {
	case CardType::Number:
		draw::setColor(232, 221, 208); break;
	case CardType::Strike:
		draw::setColor(200, 212, 240); break;
	case CardType::Flip:
		draw::setColor(224, 212, 240); break;
	case CardType::Snatch:
		draw::setColor(240, 220, 200); break;
	case CardType::Double:
		draw::setColor(232, 200, 248); break;
	}

	draw::fillRect(x, y, w, h);

	drawTexture(x, y, w, h, card, inverted);

	// Outline
	switch (card.type) {
	case CardType::Number: draw::setColor(176, 144, 112); break;
	case CardType::Strike: draw::setColor(112, 144, 208); break;
	case CardType::Flip: draw::setColor(144, 112, 192); break;
	case CardType::Snatch: draw::setColor(208, 144, 80); break;
	case CardType::Double: draw::setColor(192, 96, 224); break;
	}

	draw::drawRect(x, y, w, h);
}

/*!
	When \a inverted is \c true, cards are laid out right-to-left from \a x.
	Otherwise they expand left-to-right.
*/
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

/*!
	The hovered card is drawn last, lifted by 20px, with a glow beneath it.
	All other cards are drawn in index order.
*/
void Board::drawHandCards(float x, float y, const std::vector<Card>& cards, int hoveredIdx) {
	for (size_t i = 0; i < cards.size(); ++i) {
		if (i == hoveredIdx) continue;
		float cx = x + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());

		drawCard(cx, y, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), cards[i]);
	}

	if (hoveredIdx >= 0 && hoveredIdx < cards.size()) {
		float cx = x + hoveredIdx * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		float cy = y - 20.f;
		draw::drawGlow(cx, cy, Layout::HAND_CARD_W(), Layout::HAND_CARD_H());
		drawCard(cx, cy, Layout::HAND_CARD_W(), Layout::HAND_CARD_H(), cards[hoveredIdx]);
	}
}

//! Draws a dark panel with opponent score at top, "VS" in center, player score at bottom.
void Board::drawScorePanel(int playerScore, int oppScore) {

	float sx = Layout::SCORE_X();
	float sy = Layout::SCORE_Y();
	float sw = Layout::SCORE_W();
	float sh = Layout::SCORE_H();

	draw::setColor(10, 31, 18, 220);
	draw::fillRect(sx, sy, sw, sh);

	draw::setColor(45, 110, 69, 180);
	draw::drawRect(sx, sy, sw, sh);

	float cx = sx + sw * 0.5f;

	SDL_Color scoreCol = { 200, 240, 216, 255 };
	SDL_Color vsCol = { 106,184,128,255 };

	int scoreFontSize = Layout::scFont(52.f);
	int vsFontSize = Layout::scFont(42.f);

	draw::drawText(std::to_string(oppScore),
		cx, sy + sh * 0.24f,
		scoreFontSize,
		scoreCol, TextAlign::Center);

	draw::drawText("VS",
		cx, sy + sh * 0.52f,
		vsFontSize,
		vsCol, TextAlign::Center);

	draw::drawText(std::to_string(playerScore),
		cx, sy + sh * 0.8f,
		scoreFontSize,
		scoreCol, TextAlign::Center);
}

/*!
	Displays "You won" in yellow or "You lost" in white over a full-screen overlay.
	Shows keyboard hints for restarting or returning to the menu below the result text.
*/
void Board::drawGameOver(GameResult result) {

	draw::drawOverlay(0, 0, Layout::WIN_W, Layout::WIN_H);

	std::string text = (result == GameResult::PlayerWin) ? "You won" : "You lost";

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color yellow = { 255, 210, 80, 255 };

	SDL_Color col = (result == GameResult::PlayerWin) ? yellow : white;

	float centerX = Layout::WIN_W * 0.5f;
	float centerY = Layout::WIN_H * 0.5f;

	draw::drawText(text, centerX, centerY - Layout::scF(40.f), Layout::scFont(120.f), col, TextAlign::Center);

	float subtitleY = centerY + Layout::scF(40.f);
	draw::drawText("R - new game", centerX, subtitleY, Layout::scFont(40.f), white, TextAlign::Center);

	subtitleY += Layout::scF(40.f);
	draw::drawText("Escape - back to menu", centerX, subtitleY, Layout::scFont(40.f), white, TextAlign::Center);
}

/*!
	Shown only during \c GamePhase::PlayerTurn. Displays the last card played by
	the opponent, and optionally the last snatched or restored card if present.
*/
void Board::drawLastPlayed(const GameState& state) {
	SDL_Color dim = { 163, 217, 184, 255 };
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color red = { 255, 100, 100, 255 };

	float x = Layout::fromRight(1920.f - 20.f);
	float y = Layout::fromTop(20.f);

	if (state.phase == GamePhase::PlayerTurn && state.lastPlayedCard.has_value() && !state.lastPlayedByPlayer) {
		draw::drawText("Last Played:", x, y, Layout::scFont(28.f), dim, TextAlign::Right);
		y += Layout::scF(40.f);

		draw::drawText(cardNameOf(state.lastPlayedCard.value()), x, y, Layout::scFont(42), white, TextAlign::Right);
		y += Layout::scF(60.f);
	}

	if (state.lastSnatchedCard.has_value()) {
		draw::drawText("Snatched:", x, y, Layout::scFont(28.f), dim, TextAlign::Right);
		y += Layout::scF(40.f);
		draw::drawText(cardNameOf(state.lastSnatchedCard.value()), x, y, Layout::scFont(42.f), red, TextAlign::Right);
	}

	if (state.lastRestoredCard.has_value()) {
		SDL_Color green = { 100, 255, 150, 255 };
		draw::drawText("Restored:", x, y, 28, dim, TextAlign::Right);
		y += Layout::scFont(40.f);

		draw::drawText(cardNameOf(state.lastRestoredCard.value()), x, y, Layout::scFont(42.f), green, TextAlign::Right);
		y += Layout::scFont(60.f);
	}
}

/*!
	Entry point for a full frame draw. Draws background, divider, decks,
	hands, fields, score panel, and conditionally: last played info,
	snatch prompt, and game-over overlay based on the current game phase.
*/
void Board::render(const GameState& state) 
{
	draw::drawBackground("assets/textures/board_background.png");
	drawDivider();

	// Decks
	drawDeckStack(Layout::PLY_DECK_X(), Layout::PLY_DECK_Y());
	drawDeckStack(Layout::OPP_DECK_X(), Layout::OPP_DECK_Y(), true);

	// Enemy hand - hover (used with Snatch Card)
	int oppHovered = (state.phase == GamePhase::SelectingSnatchTarget ? m_snatchHoveredCard : -1);

	// Enemy hand
	drawHandStack(Layout::OPP_HAND_X(), Layout::OPP_HAND_Y(), static_cast<int>(state.opponent.hand.size()), oppHovered);

	// Game fields
	drawFieldCards(Layout::OPP_FIELD_X(), Layout::OPP_FIELD_Y(), state.opponent.field, true);
	drawFieldCards(Layout::PLY_FIELD_X(), Layout::PLY_FIELD_Y(), state.player.field);

	int plyHovered = (state.phase == GamePhase::PlayerTurn) ? m_hoveredCard : -1;

	// Player hand
	drawHandCards(Layout::PLY_HAND_X(), Layout::PLY_HAND_Y(), state.player.hand, plyHovered);

	drawScorePanel(state.player.score, state.opponent.score);

	if (state.phase == GamePhase::PlayerTurn)
		drawLastPlayed(state);

	if (state.phase == GamePhase::SelectingSnatchTarget)
		drawSnatchPrompt();

	if (state.phase == GamePhase::GameOver)
		drawGameOver(state.result);
}

//! Draws a semi-transparent overlay strip behind the opponent's hand with instructional text.
void Board::drawSnatchPrompt() {
	float rectY = Layout::OPP_HAND_Y() - Layout::scF(40.f);
	float rectH = Layout::HAND_CARD_H() - Layout::scF(80.f);

	draw::drawOverlay(0, rectY, Layout::WIN_W, rectH);

	float textX = Layout::WIN_W * 0.5f;
	float textY = Layout::OPP_HAND_Y();

	int fontSize = Layout::scFont(36.f);

	SDL_Color white = { 255, 255, 255, 255 };
	draw::drawText("Select an opponent's card to remove", 
		textX, textY, fontSize, white, TextAlign::Center);
}

//! Returns the card's numeric value as a string for Number cards, or the type name otherwise.
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