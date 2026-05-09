#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <string>

class TextureManager {
public:
	TextureManager(SDL_Renderer* renderer);
	~TextureManager();

	SDL_Texture* get(const std::string& path);

private:
	SDL_Renderer* m_renderer;
	std::unordered_map<std::string, SDL_Texture*> m_cache;
};