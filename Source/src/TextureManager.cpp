#include "TextureManager.h"

TextureManager::TextureManager(SDL_Renderer* renderer) : m_renderer(renderer) {}

TextureManager::~TextureManager() {
	clearCache();
}

SDL_Texture* TextureManager::get(const std::string& path) {
	auto it = m_cache.find(path);
	if (it != m_cache.end())
		return it->second;

	SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
	if (!texture) {
		SDL_Log("TextureManager: cannot load: %s", path.c_str());
	}

	m_cache[path] = texture;
	return texture;
}

void TextureManager::clearCache() {
	for (auto& [path, texture] : m_cache) {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
	m_cache.clear();
	SDL_Log("TextureManager: texture cache cleared");
}