/*!
	\file DrawUtil.cpp
	\brief Implementation of the draw namespace utilities

	Maintains three module-level pointers (renderer, TextureManager, TextRenderer)
	shared across all draw calls. All functions silently no-op if required
	pointers are null.
*/

#include "DrawUtil.h"
#include "Layout.h"

namespace draw {

	//! Stores the pointer set via \c setRenderer()
	static SDL_Renderer* s_renderer = nullptr;

	//! Stores the pointer set via \c setTextureManager()
	static TextureManager* s_textureManager = nullptr;

	//! Stores the pointer set via \c setTextRenderer()
	static TextRenderer* s_textRenderer = nullptr;

	//! Sets \c s_renderer; must be called before any draw functions
	void setRenderer(SDL_Renderer* renderer) {
		s_renderer = renderer;
	}

	//! Sets \c s_textureManager; required by \c drawTexture() and \c drawBackground(const std::string&)
	void setTextureManager(TextureManager* textureManager) {
		s_textureManager = textureManager;
	}

	//! Sets \c s_textRenderer; required by \c drawText()
	void setTextRenderer(TextRenderer* textRenderer) {
		s_textRenderer = textRenderer;
	}

	//! No-ops if \c s_renderer is null.
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (!s_renderer) return;
		SDL_SetRenderDrawColor(s_renderer, r, g, b, a);
	}

	//! No-ops if \c s_renderer is null.
	void fillRect(float x, float y, float w, float h) {
		if (!s_renderer) return;
		SDL_FRect rc{ x,y,w,h };
		SDL_RenderFillRect(s_renderer, &rc);
	}

	//! No-ops if \c s_renderer is null.
	void drawRect(float x, float y, float w, float h) {
		if (!s_renderer) return;
		SDL_FRect rc{ x,y,w,h };
		SDL_RenderRect(s_renderer, &rc);
	}

	/*!
		Uses \c SDL_RenderTexture() when \a angle is 0 and \a center is null,
		otherwise falls back to \c SDL_RenderTextureRotated().
		No-ops if renderer or TextureManager is null, or if the texture cannot be resolved.
	*/
	void drawTexture(const std::string& texturePath, float x, float y, float w, float h, float angle, const SDL_FPoint* center) {
		if (!s_renderer || !s_textureManager) return;

		SDL_Texture* texture = s_textureManager->get(texturePath);
		if (!texture) return;

		SDL_FRect dst{ x,y,w,h };
		if (angle == 0.0f && center == nullptr)
			SDL_RenderTexture(s_renderer, texture, nullptr, &dst);
		else
			SDL_RenderTextureRotated(s_renderer, texture, nullptr, &dst, angle, center, SDL_FLIP_NONE);
	}

	//! No-ops if \c s_textRenderer is null.
	void drawText(const std::string& text, float x, float y, int size, SDL_Color color, TextAlign align, int wrapWidth) {
		if (!s_textRenderer) return;
		s_textRenderer->draw(text, x, y, size, color, align, wrapWidth);
	}

	/*!
		Draws three concentric filled rectangles expanding outward from the target rect,
		each with increasing alpha to produce a soft glow effect.
		Temporarily enables \c SDL_BLENDMODE_BLEND.
	*/
	void drawGlow(float x, float y, float w, float h) {
		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);

		int layers = 3;
		for (int i = layers; i >= 1; --i) {
			float expand = i * 6.f;
			float alpha = 40.f + (layers - i) * 15.f;

			draw::setColor(255, 220, 80, static_cast<uint8_t>(alpha));
			draw::fillRect(x - expand, y - expand, w + expand * 2, h + expand * 2);
		}

		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_NONE);
	}

	//! Temporarily enables \c SDL_BLENDMODE_BLEND for the fill.
	void drawOverlay(float x, float y, float w, float h) {
		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
		draw::setColor(0, 0, 0, 210);
		draw::fillRect(x, y, w, h);
		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_NONE);
	}

	//! No-ops if \c s_renderer is null.
	void drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (!s_renderer) return;
		setColor(r, g, b, a);
		fillRect(0, 0, Layout::WIN_W, Layout::WIN_H);
	}

	//! No-ops if \c s_renderer or \c s_textureManager is null.
	void drawBackground(const std::string& texturePath) {
		if (!s_renderer || !s_textureManager) return;
		drawTexture(texturePath, 0, 0, Layout::WIN_W, Layout::WIN_H);
	}
}