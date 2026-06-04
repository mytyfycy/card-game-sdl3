#pragma once
#include <SDL3/SDL.h>
#include "TextRenderer.h"
#include "TextureManager.h"
#include "Subscribable.h"
#include "StateManager.h"

enum class SelectState {
	Main,
	DifficultySelect,
	Tutorial
};

enum class ArrowState {
	None,
	Left,
	Right
};

class MainMenu : public Subscribable {
public:
	MainMenu(SDL_Renderer* renderer, StateManager* stateManager);
	void handleEvent(const SDL_Event& e);
	void update();
	void render();

	TextRenderer* getTextRenderer() { return &m_text; }
	TextureManager* getTextureManager() { return &m_textures; }

private:
	int m_hoveredButton = -1;
	int m_hoveredOverlayButton = -1;

	int m_currentTutorialPage = 0;
	ArrowState m_hoveredTutorialArrow = ArrowState::None;

	SelectState m_currentSelectState = SelectState::Main;

	SDL_Renderer* m_renderer;
	TextRenderer m_text;
	TextureManager m_textures;
	StateManager* m_stateManager;

	std::vector<std::string> m_buttonLabels;
	size_t m_buttonLabelsSize;

	std::vector<std::string> m_difficultyLabels;
	size_t m_difficultyLabelsSize;

	std::vector<std::string> m_tutorialPages;

	void drawTitle();

	void drawDifficultySelection();

	SDL_FRect getOverlayRect();
	SDL_FRect getOverlayButtonRect(int index, int totalButtons);

	void drawButton(const std::string& text, const SDL_FRect& rect);
	SDL_FRect getButtonRect(int index);
	void handleButtonClick(int index);

	void bindEvents();

	void initTutorial();
	SDL_FRect getLeftArrowRect();
	SDL_FRect getRightArrowRect();
};