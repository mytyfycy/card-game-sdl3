/*!
	\file TextureManager.cpp
	\brief Implementation of TextureManager
*/

#include "TextureManager.h"

//! Stores \a renderer for use in \c get()
TextureManager::TextureManager(SDL_Renderer* renderer) : m_renderer(renderer) {}

//! Calls \c clearCache()
TextureManager::~TextureManager() {
	clearCache();
}

/*!
	Returns the cached texture if already loaded. Otherwise calls
	\c IMG_LoadTexture(), logs an error on failure, stores the result
	(even if null) in the cache, and returns it.
*/
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

//! Calls \c SDL_DestroyTexture() on each entry, clears the map, and logs the operation.
void TextureManager::clearCache() {
	for (auto& [path, texture] : m_cache) {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
	m_cache.clear();
	SDL_Log("TextureManager: texture cache cleared");
}