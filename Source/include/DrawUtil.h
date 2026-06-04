#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "TextureManager.h"
#include "TextRenderer.h"

namespace draw {
	void setRenderer(SDL_Renderer* renderer);
	void setTextRenderer(TextRenderer* textRenderer);
	void setTextureManager(TextureManager* textureManager);

	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void fillRect(float x, float y, float w, float h);
	void drawRect(float x, float y, float w, float h);

	void drawTexture(const std::string& texturePath, float x, float y, float w, float h, float angle = 0.0f, const SDL_FPoint* center = nullptr);
	void drawText(const std::string& text, float x, float y, int size, SDL_Color color, TextAlign align = TextAlign::Left, int wrapWidth = 0);

	void drawGlow(float x, float y, float w, float h);

	void drawOverlay(float x, float y, float w, float h);

	void drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void drawBackground(const std::string& texturePath);
}