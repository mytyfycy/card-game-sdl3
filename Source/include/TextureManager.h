#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <string>

/*!
	\class TextureManager
	\brief Loads and caches SDL textures by file path

	Textures are loaded on first access and reused on subsequent calls
	to \c get(). All cached textures are destroyed when \c clearCache()
	is called or the manager is destroyed.
*/
class TextureManager {
public:
	/*!
		\brief Constructs the TextureManager with the given SDL renderer
		\param renderer Renderer used to load textures via \c IMG_LoadTexture()
	*/
	TextureManager(SDL_Renderer* renderer);

	//! Destroys all cached textures
	~TextureManager();

	/*!
		\brief Returns the texture for the given path, loading it if not yet cached
		\param path File path to the image asset
		\return Pointer to the loaded \c SDL_Texture, or \c nullptr on failure
	*/
	SDL_Texture* get(const std::string& path);

	//! Destroys all cached textures and clears the cache
	void clearCache();

private:
	SDL_Renderer* m_renderer; //!< Renderer used for texture loading
	std::unordered_map<std::string, SDL_Texture*> m_cache; //!< Map of file paths to loaded textures
};