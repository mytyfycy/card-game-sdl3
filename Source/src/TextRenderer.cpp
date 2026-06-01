#include "TextRenderer.h"

TextRenderer::TextRenderer(SDL_Renderer* renderer, const char* fontPath)
	: m_renderer(renderer), m_font(nullptr), m_fontPath(fontPath) {

	if (!TTF_WasInit()) {
		TTF_Init();
	}

	m_font = TTF_OpenFont(fontPath, 32);
	if (!m_font)
		SDL_Log("TextRenderer: cannot load font: %s", fontPath);
}

TextRenderer::~TextRenderer() {
	for (auto& pair : m_textureCache) {
		if (pair.second.texture) {
			SDL_DestroyTexture(pair.second.texture);
		}
	}
	m_textureCache.clear();

	for (auto& pair : m_fontSizesCache) {
		if (pair.second) {
			TTF_CloseFont(pair.second);
		}
	}
	m_fontSizesCache.clear();

	if (m_font)
		TTF_CloseFont(m_font);

	TTF_Quit();
}

TTF_Font* TextRenderer::getFont(int size) {
	if (size <= 0) size = 1;

	auto it = m_fontSizesCache.find(size);
	if (it != m_fontSizesCache.end())
		return it->second;

	TTF_Font* newFont = TTF_OpenFont(m_fontPath.c_str(), size);

	if (!newFont) {
		SDL_Log("TextRenderer: cannot load font size");
		return m_font;
	}

	m_fontSizesCache[size] = newFont;
	return newFont;
}

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

void TextRenderer::draw(const std::string& text,
	float x, float y,
	int fontSize,
	SDL_Color color, TextAlign align) {
	if (!m_font || text.empty()) return;

	TextCacheKey key = std::make_tuple(text, fontSize, color.r, color.g, color.b);

	auto it = m_textureCache.find(key);
	CachedTexture cached;

	if (it == m_textureCache.end()) {
		TTF_Font* f = getFont(fontSize);
		SDL_Surface* surf = TTF_RenderText_Blended(f, text.c_str(), text.size(), color);
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

	SDL_FRect dst{ dx, y - cached.h / 2.f, cached.w, cached.h };

	SDL_RenderTexture(m_renderer, cached.texture, nullptr, &dst);
}

void TextRenderer::clearCache() {
	for (auto& pair : m_textureCache) {
		if (pair.second.texture)
			SDL_DestroyTexture(pair.second.texture);
	}
	m_textureCache.clear();

	SDL_Log("TextRenderer: font cache cleared");
}