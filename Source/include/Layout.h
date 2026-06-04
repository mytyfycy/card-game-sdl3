#pragma once
#include <SDL3/SDL.h>

/*!
	\namespace Layout
	\brief Resolution-independent layout system for all on-screen positions and sizes

	All coordinates are defined relative to a 1920x1080 base resolution.
	Call \c update() whenever the window is resized to keep \c WIN_W, \c WIN_H,
	and \c safeScale current. Use the provided helper functions instead of
	hardcoded pixel values throughout the codebase.
*/
namespace Layout {

	inline constexpr float BASE_W = 1920.f; //!< Reference design width in pixels
	inline constexpr float BASE_H = 1080.f; //!< Reference design height in pixels

	extern float WIN_W; //!< Current window width in pixels
	extern float WIN_H; //!< Current window height in pixels
	extern float safeScale; //!< Uniform scale factor derived from WIN_H / BASE_H

	//! \name Scaling helpers
	//! \{

	/*!
		\brief Scales a base value by \c safeScale
		\param v Value defined at 1920x1080
		\return Scaled value for the current window size
	*/
	float scF(float v);

	/*!
		\brief Returns a font size scaled by the average of the horizontal and vertical scale factors
		\param baseSize Font size defined at 1920x1080
		\return Integer font size suitable for the current window size
	*/
	int scFont(float baseSize);

	//! \}

	//! \name Anchor helpers
	//! \{

	//! Returns a position offset from the left edge, scaled by \c safeScale
	float fromLeft(float baseVal);

	//! Returns a position offset from the right edge, scaled by \c safeScale
	float fromRight(float baseVal);

	//! Returns a position offset from the top edge, scaled by \c safeScale
	float fromTop(float baseVal);

	//! Returns a position offset from the bottom edge, scaled by \c safeScale
	float fromBottom(float baseVal);

	//! Returns a position offset from the horizontal center, scaled by \c safeScale
	float fromCenterX(float baseVal);

	//! Returns a position offset from the vertical center, scaled by \c safeScale
	float fromCenterY(float baseVal);

	//! \}

	//! \name Card dimensions
	//! \{
	
	float CARD_W(); //!< Scaled field card width
	float CARD_H(); //!< Scaled field card height
	float CARD_GAP(); //!< Scaled horizontal gap between field cards (negative = overlap)

	//! \}

	//! \name Player deck position
	//! \{
	float PLY_DECK_X(); //!< Left edge of the player's deck stack
	float PLY_DECK_Y(); //!< Top edge of the player's deck stack
	//! \}

	//! \name Opponent deck position
	//! \{
	float OPP_DECK_X(); //!< Left edge of the opponent's deck stack
	float OPP_DECK_Y(); //!< Top edge of the opponent's deck stack

	//! \}

	//! \name Player hand position
	//! \{
	float PLY_HAND_X(); //!< Left edge of the player's hand row
	float PLY_HAND_Y(); //!< Top edge of the player's hand row

	//! \}

	//! \name Opponent hand position
	//! \{
	float OPP_HAND_X(); //!< Left edge of the opponent's hand row
	float OPP_HAND_Y(); //!< Top edge of the opponent's hand row

	//! \}

	//! \name Hand card dimensions
	//! \{
	float HAND_CARD_W(); //!< Scaled hand card width
	float HAND_CARD_H(); //!< Scaled hand card height
	float HAND_GAP(); //!< Scaled horizontal gap between hand cards (negative = overlap)
	//! \}

	//! \name Player field position
	//! \{
	float PLY_FIELD_X(); //!< Left edge of the player's field row
	float PLY_FIELD_Y(); //!< Top edge of the player's field row
	//! \}

	//! \name Opponent field position
	//! \{
	float OPP_FIELD_X(); //!< Left edge of the opponent's field row
	float OPP_FIELD_Y(); //!< Top edge of the opponent's field row

	//! \}

	//! \name Score panel position and size
	//! \{
	float SCORE_X(); //!< Left edge of the score panel
	float SCORE_Y(); //!< Top edge of the score panel
	float SCORE_W(); //!< Scaled score panel width
	float SCORE_H(); //!< Scaled score panel height
	//! \}

	//! Y coordinate of the horizontal divider line across the board
	float DIVIDER_Y();

	/*!
		\brief Updates \c WIN_W, \c WIN_H, and \c safeScale from the current window size
		\param window Window to query; should be called on every resize event
	*/
	void update(SDL_Window* window);
}