#pragma once
#include "GameStateBase.h"
#include "MainMenu.h"
#include <memory>
#include "SDL3/SDL.h"
#include "AudioSystem.h"

class MenuState : public GameStateBase {
public:
	MenuState(SDL_Renderer* renderer, StateManager* stateManager);

	void onEnter() override;
	void onExit() override;
	void handleEvent(const SDL_Event& e) override;
	void update() override;
	void render() override;

private:
	SDL_Renderer* m_renderer;
	StateManager* m_stateManager;

	std::unique_ptr<MainMenu> m_menu;
	std::unique_ptr<AudioSystem> m_audio;
};