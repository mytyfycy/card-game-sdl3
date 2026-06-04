#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <map>
#include <tuple>

enum class TextAlign { Left, Center, Right };

class TextRenderer {
public:
	TextRenderer(SDL_Renderer* renderer, const char* fontPath);
	~TextRenderer();

	void draw(const std::string& text,
		float x, float y,
		int fontSize,
		SDL_Color color,
		TextAlign align = TextAlign::Left,
		int wrapWidth = 0);

	SDL_FPoint measure(const std::string& text, int fontSize);

	void clearCache();

private:
	TTF_Font* getFont(int size);

	SDL_Renderer* m_renderer;
	TTF_Font* m_font;
	std::string m_fontPath;

	std::map<int, TTF_Font*> m_fontSizesCache;

	using TextCacheKey = std::tuple<std::string, int, uint8_t, uint8_t, uint8_t>;

	struct CachedTexture {
		SDL_Texture* texture;
		float w;
		float h;
	};

	std::map<TextCacheKey, CachedTexture> m_textureCache;
};