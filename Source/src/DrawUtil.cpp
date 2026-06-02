#include "DrawUtil.h"
#include "Layout.h"

namespace draw {
	static SDL_Renderer* s_renderer = nullptr;
	static TextureManager* s_textureManager = nullptr;
	static TextRenderer* s_textRenderer = nullptr;

	void setRenderer(SDL_Renderer* renderer) {
		s_renderer = renderer;
	}

	void setTextureManager(TextureManager* textureManager) {
		s_textureManager = textureManager;
	}

	void setTextRenderer(TextRenderer* textRenderer) {
		s_textRenderer = textRenderer;
	}

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (!s_renderer) return;
		SDL_SetRenderDrawColor(s_renderer, r, g, b, a);
	}

	void fillRect(float x, float y, float w, float h) {
		if (!s_renderer) return;
		SDL_FRect rc{ x,y,w,h };
		SDL_RenderFillRect(s_renderer, &rc);
	}

	void drawRect(float x, float y, float w, float h) {
		if (!s_renderer) return;
		SDL_FRect rc{ x,y,w,h };
		SDL_RenderRect(s_renderer, &rc);
	}

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

	void drawText(const std::string& text, float x, float y, int size, SDL_Color color, TextAlign align) {
		if (!s_textRenderer) return;
		s_textRenderer->draw(text, x, y, size, color, align);
	}

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

	void drawOverlay(float x, float y, float w, float h) {
		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
		draw::setColor(0, 0, 0, 160);
		draw::fillRect(x, y, w, h);
		SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_NONE);
	}

	void drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (!s_renderer) return;
		setColor(r, g, b, a);
		fillRect(0, 0, Layout::WIN_W, Layout::WIN_H);
	}

	void drawBackground(const std::string& texturePath) {
		if (!s_renderer || !s_textureManager) return;
		drawTexture(texturePath, 0, 0, Layout::WIN_W, Layout::WIN_H);
	}
}