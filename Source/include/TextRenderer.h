#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <map>
#include <tuple>

/*!
	\enum TextAlign
	\brief Horizontal alignment of rendered text relative to the draw position

	\var TextAlign::Left    Text starts at the given x coordinate
	\var TextAlign::Center  Text is centered on the given x coordinate
	\var TextAlign::Right   Text ends at the given x coordinate
*/
enum class TextAlign { Left, Center, Right };

/*!
	\class TextRenderer
	\brief Renders and caches SDL text textures with per-size font support

	Fonts are opened lazily per pixel size and cached in \c m_fontSizesCache.
	Rendered text textures are cached by (text, size, color) key to avoid
	redundant surface and texture creation. Each draw call renders a drop
	shadow followed by the text itself.
*/
class TextRenderer {
public:
	/*!
		\brief Constructs TextRenderer and opens the font at size 32 as a fallback
		\param renderer SDL renderer used to create text textures
		\param fontPath Path to the TTF font file
	*/
	TextRenderer(SDL_Renderer* renderer, const char* fontPath);

	//! Clears the texture cache and closes all open font instances
	~TextRenderer();

	/*!
		\brief Renders a string at the given position
		\param text      String to render
		\param x         Horizontal anchor position
		\param y         Vertical center position
		\param fontSize  Font size in pixels
		\param color     Text color; alpha is ignored in the cache key
		\param align     Horizontal alignment relative to \a x (default: Left)
		\param wrapWidth Maximum line width in pixels before wrapping; 0 disables wrapping
	*/
	void draw(const std::string& text,
		float x, float y,
		int fontSize,
		SDL_Color color,
		TextAlign align = TextAlign::Left,
		int wrapWidth = 0);

	/*!
		\brief Returns the rendered size of a string at the given font size
		\param text     String to measure
		\param fontSize Font size in pixels
		\return Width and height in pixels as \c SDL_FPoint
	*/
	SDL_FPoint measure(const std::string& text, int fontSize);

	//! Destroys all cached text textures and clears the texture cache
	void clearCache();

private:
	/*!
		\brief Returns a font instance for the given size, opening it if not yet cached
		\param size Font size in pixels; clamped to a minimum of 1
		\return Cached or newly opened \c TTF_Font; falls back to \c m_font on failure
	*/
	TTF_Font* getFont(int size);

	SDL_Renderer* m_renderer; //!< SDL renderer used to create textures from surfaces
	TTF_Font* m_font; //!< Fallback font instance opened at size 32
	std::string m_fontPath; //!< Path to the TTF font file, retained for lazy font loading

	//! Cache of TTF_Font instances keyed by pixel size
	std::map<int, TTF_Font*> m_fontSizesCache;

	//! Cache key: (text, fontSize, r, g, b)
	using TextCacheKey = std::tuple<std::string, int, uint8_t, uint8_t, uint8_t>;

	/*!
		\brief Cached rendered text texture with its dimensions
	*/
	struct CachedTexture {
		SDL_Texture* texture; //!< Rendered text texture
		float w; //!< Texture width in pixels
		float h; //!< Texture height in pixels
	};

	//! Cache of rendered text textures keyed by (text, fontSize, r, g, b)
	std::map<TextCacheKey, CachedTexture> m_textureCache;
};