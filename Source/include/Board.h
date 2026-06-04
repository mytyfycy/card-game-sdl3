#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "TextRenderer.h"
#include "TextureManager.h"

/*!
	\class Board
	\brief Renders all in-game visual elements from a GameState snapshot

	Draws the player and opponent hands, field cards, deck stacks,
	score panel, and overlay screens (game over, snatch prompt).
	Owns a TextRenderer and TextureManager used throughout the game view.
*/
class Board {
public:
	/*!
		\brief Constructs the Board and sets up rendering resources
		\param renderer SDL renderer to draw with
		\param fontPath Path to the font file used for all text
	*/
	Board(SDL_Renderer* renderer, const char* fontPath);

	/*!
		\brief Draws the full game view based on current state
		\param state Snapshot of the game state to render
	*/
	void render(const GameState& state);

	/*!
		\brief Sets the index of the player's currently hovered hand card
		\param index Card index, or -1 if none
	*/
	void onCardHovered(int index) { m_hoveredCard = index; }

	/*!
		\brief Sets the index of the opponent's card hovered during a Snatch action
		\param index Card index, or -1 if none
	*/
	void onSnatchHovered(int index) { m_snatchHoveredCard = index; }

	//! Clears the text cache
	void clearTextureCache() { m_text.clearCache(); }

	//! Clears the texture cache
	void clearTextCache() { m_textures.clearCache(); }

	//! Returns a pointer to the internal TextRenderer
	TextRenderer* getTextRenderer() { return &m_text; }

	//! Returns a pointer to the internal TextureManager
	TextureManager* getTextureManager() { return &m_textures; }

private:
	int m_hoveredCard = -1; //!< Index of the hovered player hand card (-1 if none)
	int m_snatchHoveredCard = -1; //!< Index of the hovered opponent card during Snatch (-1 if none)
	SDL_Renderer* m_renderer; //!< SDL renderer used for all draw calls
	TextRenderer m_text; //!< Text rendering helper
	TextureManager m_textures; //!< Texture caching and lookup
	
	//! Draws a dashed horizontal divider across the board at mid-field positions
	void drawDivider();

	/*!
		\brief Draws a stacked deck visual at the given position
		\param x        Left edge of the deck
		\param y        Top edge of the deck
		\param inverted If \c true, draws the deck rotated 180 degrees
	*/
	void drawDeckStack(float x, float y, bool inverted = false);

	/*!
		\brief Draws a row of face-down hand cards with optional hover highlight
		\param x          Left edge of the first card
		\param y          Top edge of the row
		\param count      Number of cards to draw
		\param hoveredIdx Index of the highlighted card, or -1 if none
	*/
	void drawHandStack(float x, float y, int count, int hoveredIdx);

	/*!
		\brief Draws a row of face-up field cards
		\param x        Starting edge (left if normal, right if inverted)
		\param y        Top edge of the row
		\param cards    Cards to draw
		\param inverted If \c true, lays cards out right-to-left and rotated 180 degrees
	*/
	void drawFieldCards(float x, float y, const std::vector<Card>& cards, bool inverted = false);

	/*!
		\brief Draws the player's hand with hover lift effect on the selected card
		\param x          Left edge of the first card
		\param y          Top edge of the row
		\param cards      Cards to draw
		\param hoveredIdx Index of the hovered card, or -1 if none
	 */
	void drawHandCards(float x, float y, const std::vector<Card>& cards, int hoveredIdx);

	/*!
		\brief Draws the score panel showing both players' current scores
		\param playerScore Player's current score
		\param oppScore    Opponent's current score
	*/
	void drawScorePanel(int playerScore, int oppScore);

	/*!
		\brief Draws the game-over overlay with result text and restart hints
		\param result Outcome to display (PlayerWin or PlayerLose)
	*/
	void drawGameOver(GameResult result);

	/*!
		\brief Draws the last played, snatched, and restored card info in the top-right corner
		\param state Current game state supplying the card history fields
	*/
	void drawLastPlayed(const GameState& state);

	/*!
		\brief Draws a single card at the given position and size
		\param x        Left edge
		\param y        Top edge
		\param w        Card width
		\param h        Card height
		\param card     Card data to render (type, value, texture)
		\param inverted If \c true, renders the card rotated 180 degrees
	*/
	void drawCard(float x, float y, float w, float h, const Card& card, bool inverted = false);

	/*!
		\brief Draws the card's texture overlay if \c card.texturePath is set
		\param x        Left edge
		\param y        Top edge
		\param w        Width
		\param h        Height
		\param card     Card whose texture to draw
		\param inverted If \c true, rotates the texture 180 degrees
	*/
	void drawTexture(float x, float y, float w, float h, const Card& card, bool inverted = false);

	//! Draws an overlay prompt asking the player to select an opponent card to snatch
	void drawSnatchPrompt();

	/*!
		\brief Returns a display name for the given card
		\param card Card to name
		\return Number value as string for Number cards, type name for special cards
	*/
	std::string cardNameOf(const Card& card) const;
};