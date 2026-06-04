#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "TextureManager.h"
#include "TextRenderer.h"

/*!
	\namespace draw
	\brief Stateless drawing utilities backed by a shared SDL renderer

	All functions operate on module-level pointers set via the \c set* functions.
	Must call \c setRenderer() before any draw calls; \c setTextureManager() and
	\c setTextRenderer() are required for texture and text functions respectively.
*/
namespace draw {

	//! \name Context setup
	//! \{

	//! Sets the SDL renderer used by all draw functions
	void setRenderer(SDL_Renderer* renderer);

	//! Sets the TextRenderer used by \c drawText()
	void setTextRenderer(TextRenderer* textRenderer);

	//! Sets the TextureManager used by \c drawTexture()
	void setTextureManager(TextureManager* textureManager);

	//! \}

	//! \name Primitives
	//! \{

	/*!
		\brief Sets the active draw color for subsequent primitive calls
		\param r Red channel
		\param g Green channel
		\param b Blue channel
		\param a Alpha channel (default: 255)
	*/
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	//! Fills a rectangle with the current draw color
	void fillRect(float x, float y, float w, float h);

	//! Draws the outline of a rectangle with the current draw color
	void drawRect(float x, float y, float w, float h);

	//! \}

	//! \name Assets
	//! \{

	/*!
		\brief Draws a texture from the TextureManager at the given position and size
        \param texturePath  Key used to look up the texture in the TextureManager
        \param x            Left edge
        \param y            Top edge
        \param w            Width
        \param h            Height
        \param angle        Clockwise rotation in degrees (default: 0)
        \param center       Rotation pivot point, or \c nullptr for the texture center
    */
	void drawTexture(const std::string& texturePath, float x, float y, float w, float h, float angle = 0.0f, const SDL_FPoint* center = nullptr);

	/*!
		\brief Draws a string via the TextRenderer
		\param text       String to render
		\param x          Horizontal anchor position
		\param y          Vertical anchor position
		\param size       Font size in pixels
		\param color      Text color
		\param align      Horizontal alignment relative to \a x (default: Left)
		\param wrapWidth  Maximum line width in pixels before wrapping; 0 disables wrapping
	*/
	void drawText(const std::string& text, float x, float y, int size, SDL_Color color, TextAlign align = TextAlign::Left, int wrapWidth = 0);

	//! \}

	//! \name Effects
	//! \{

	/*!
		\brief Draws a multi-layer yellow glow around a rectangle
		\param x  Left edge of the target rectangle
		\param y  Top edge of the target rectangle
		\param w  Width of the target rectangle
		\param h  Height of the target rectangle
	*/
	void drawGlow(float x, float y, float w, float h);

	/*!
		\brief Draws a semi-transparent black overlay over a rectangle
		\param x  Left edge
		\param y  Top edge
		\param w  Width
		\param h  Height
	*/
	void drawOverlay(float x, float y, float w, float h);

	//! \}

	//! \name Background
	//! \{

	/*!
		\brief Fills the entire window with a solid color
		\param r Red channel
		\param g Green channel
		\param b Blue channel
		\param a Alpha channel (default: 255)
	*/
	void drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	/*!
		\brief Draws a texture stretched to fill the entire window
		\param texturePath Key used to look up the texture in the TextureManager
	*/
	void drawBackground(const std::string& texturePath);

	//! \}
}