/*!
	\file TextRenderer.cpp
	\brief Implementation of TextRenderer
*/

#include "TextRenderer.h"

/*!
	Opens the font at size 32 as a fallback for \c getFont() failures.
	Logs an error if the font cannot be loaded.
*/
TextRenderer::TextRenderer(SDL_Renderer* renderer, const char* fontPath)
	: m_renderer(renderer), m_font(nullptr), m_fontPath(fontPath) {

	m_font = TTF_OpenFont(fontPath, 32);
	if (!m_font)
		SDL_Log("TextRenderer: cannot load font: %s", fontPath);
}

/*!
	Calls \c clearCache() to destroy all text textures, then closes and
	removes all per-size font instances from \c m_fontSizesCache,
	and finally closes the fallback \c m_font.
*/
TextRenderer::~TextRenderer() {
	clearCache();

	for (auto& pair : m_fontSizesCache) {
		if (pair.second) {
			TTF_CloseFont(pair.second);
		}
	}
	m_fontSizesCache.clear();

	if (m_font)
		TTF_CloseFont(m_font);
}

/*!
	Clamps \a size to a minimum of 1. Returns the cached font if available,
	otherwise opens a new instance via \c TTF_OpenFont(), caches and returns it.
	Falls back to \c m_font and logs an error if opening fails.
*/
TTF_Font* TextRenderer::getFont(int size) {
	if (size <= 0) size = 1;

	auto it = m_fontSizesCache.find(size);
	if (it != m_fontSizesCache.end())
		return it->second;

	TTF_Font* newFont = TTF_OpenFont(m_fontPath.c_str(), static_cast<float>(size));

	if (!newFont) {
		SDL_Log("TextRenderer: cannot load font size");
		return m_font;
	}

	m_fontSizesCache[size] = newFont;
	return newFont;
}

//! Returns \c {0, 0} for empty strings. Uses \c TTF_GetStringSize() internally.
SDL_FPoint TextRenderer::measure(const std::string& text, int fontSize) {
	if (text.empty()) return { 0.f, 0.f };

	TTF_Font* f = getFont(fontSize);
	int w = 0;
	int h = 0;
	TTF_GetStringSize(f, text.c_str(), text.size(), &w, &h);
	return {
		static_cast<float>(w),
		static_cast<float>(h)
	};
}

/*!
	Looks up the (text, fontSize, r, g, b) key in \c m_textureCache. On a miss,
	renders via \c TTF_RenderText_Blended_Wrapped(), creates an SDL texture, and
	stores it. Draws a semi-transparent black shadow offset by \c fontSize * 0.05
	before drawing the final texture. Text is vertically centered on \a y and
	horizontally aligned according to \a align.
*/
void TextRenderer::draw(const std::string& text,
	float x, float y,
	int fontSize,
	SDL_Color color, TextAlign align,
	int wrapWidth) {
	if (!m_font || text.empty()) return;

	TextCacheKey key = std::make_tuple(text, fontSize, color.r, color.g, color.b);

	auto it = m_textureCache.find(key);
	CachedTexture cached;

	if (it == m_textureCache.end()) {
		TTF_Font* f = getFont(fontSize);
		SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(f, text.c_str(), text.size(), color, wrapWidth);
		if (!surf) return;

		SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
		float w = static_cast<float>(surf->w);
		float h = static_cast<float>(surf->h);
		SDL_DestroySurface(surf);

		if (!tex) return;

		cached.texture = tex;
		cached.w = w;
		cached.h = h;
		m_textureCache[key] = cached;
	}
	else {
		cached = it->second;
	}

	float dx = x;
	if (align == TextAlign::Center) dx = x - cached.w / 2.f;
	else if (align == TextAlign::Right) dx = x - cached.w;

	float offset = std::max(1.f, fontSize * 0.05f);
	SDL_FRect shadowDst{ dx + offset, (y - cached.h * 0.5f) + offset, cached.w, cached.h };

	SDL_SetTextureColorMod(cached.texture, 0, 0, 0);
	SDL_SetTextureAlphaMod(cached.texture, 128);
	SDL_RenderTexture(m_renderer, cached.texture, nullptr, &shadowDst);

	SDL_SetTextureColorMod(cached.texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(cached.texture, 255);

	SDL_FRect dst{ dx, y - cached.h / 2.f, cached.w, cached.h };

	SDL_RenderTexture(m_renderer, cached.texture, nullptr, &dst);
}

//! Destroys all textures in \c m_textureCache, clears the map, and logs the operation.
void TextRenderer::clearCache() {
	for (auto& pair : m_textureCache) {
		if (pair.second.texture)
			SDL_DestroyTexture(pair.second.texture);
	}
	m_textureCache.clear();

	SDL_Log("TextRenderer: font cache cleared");
}