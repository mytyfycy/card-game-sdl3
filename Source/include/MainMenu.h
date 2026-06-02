#pragma once
#include <SDL3/SDL.h>
#include "TextRenderer.h"
#include "TextureManager.h"
#include "Subscribable.h"
#include "StateManager.h"

class MainMenu : public Subscribable {
public:
	MainMenu(SDL_Renderer* renderer, StateManager* stateManager);
	void handleEvent(const SDL_Event& e);
	void update();
	void render();

	TextRenderer* getTextRenderer() { return &m_text; }
	TextureManager* getTextureManager() { return &m_textures; }

private:
	int m_hoveredOption = -1;
	SDL_Renderer* m_renderer;
	TextRenderer m_text;
	TextureManager m_textures;
	StateManager* m_stateManager;

	std::vector<std::string> m_buttonLabels = { "Play", "Rules", "Quit" };

	void drawTitle();

	void drawButton(const std::string& text, const SDL_FRect& rect);
	SDL_FRect getButtonRect(int index);
	void handleButtonClick(int index);

	void bindEvents();
};