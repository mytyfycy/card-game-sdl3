#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

enum class TextAlign { Left, Center, Right };

class TextRenderer {
public:
	TextRenderer(SDL_Renderer* renderer, const char* fontPath);
	~TextRenderer();

	void draw(const std::string& text,
		float x, float y,
		int fontSize,
		SDL_Color color,
		TextAlign align = TextAlign::Left);

	SDL_FPoint measure(const std::string& text, int fontSize);

	bool isValid() const { return m_font != nullptr; }

private:
	SDL_Renderer* m_renderer;
	TTF_Font* m_font;
	int m_loadedSize;

	TTF_Font* getFont(int size);
	TTF_Font* m_cachedFont = nullptr;
	int m_cachedSize = 0;
};