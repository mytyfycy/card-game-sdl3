#include "TextRenderer.h"
#include <SDL3_ttf/SDL_ttf.h>

TextRenderer::TextRenderer(SDL_Renderer* renderer, const char* fontPath)
	: m_renderer(renderer), m_font(nullptr), m_loadedSize(0)
{
	if (!TTF_WasInit())
		TTF_Init();

	m_font = TTF_OpenFont(fontPath, 32);
	if (!m_font)
		SDL_Log("TextRenderer: cannot load font: %s", fontPath);
}

TextRenderer::~TextRenderer() {
	if (m_cachedFont) TTF_CloseFont(m_cachedFont);
	if (m_font) TTF_CloseFont(m_font);
	TTF_Quit();
}

TTF_Font* TextRenderer::getFont(int size) {
	if (m_cachedFont && m_cachedSize == size)
		return m_cachedFont;

	if (m_cachedFont)
		TTF_CloseFont(m_cachedFont);

	m_cachedFont = TTF_OpenFont(
		TTF_GetFontFamilyName(m_font), size);
	m_cachedSize = size;
	return m_cachedFont ? m_cachedFont : m_font;
}

SDL_FPoint TextRenderer::measure(const std::string& text, int fontSize) {
	TTF_Font* f = getFont(fontSize);
	int w = 0;
	int h = 0;
	TTF_GetStringSize(f, text.c_str(), text.size(), &w, &h);
	return { static_cast<float>(w), static_cast<float>(h) };
}

void TextRenderer::draw(const std::string& text,
	float x, float y,
	int fontSize,
	SDL_Color color,
	TextAlign align)
{
	if (!m_font) return;

	TTF_Font* f = getFont(fontSize);
	SDL_Surface* surf = TTF_RenderText_Blended(
		f, text.c_str(), text.size(), color);
	if (!surf) return;

	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
	SDL_DestroySurface(surf);
	if (!tex) return;

	float w = static_cast<float>(tex->w);
	float h = static_cast<float>(tex->h);

	float dx = x;
	if (align == TextAlign::Center) dx = x - w / 2.f;
	else if (align == TextAlign::Right) dx = x - w;

	SDL_FRect dst{ dx, y - h / 2.f, w ,h };
	SDL_RenderTexture(m_renderer, tex, nullptr, &dst);
	SDL_DestroyTexture(tex);
}