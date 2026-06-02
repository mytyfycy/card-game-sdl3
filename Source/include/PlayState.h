#pragma once
#include "GameStateBase.h"
#include "Game.h"
#include "AudioSystem.h"
#include "UISystem.h"
#include "DebugSystem.h"
#include "Difficulty.h"

class PlayState : public GameStateBase {
public:
	PlayState(SDL_Renderer* renderer, Difficulty difficulty);

	void onEnter() override;
	void onExit() override;
	void handleEvent(const SDL_Event& e) override;
	void update() override;
	void render() override;

private:
	SDL_Renderer* m_renderer;
	Difficulty m_difficulty;

	std::unique_ptr<Game> m_game;
	std::unique_ptr<AudioSystem> m_audio;
	std::unique_ptr<UISystem> m_ui;
#ifdef _DEBUG
	std::unique_ptr<DebugSystem> m_debug;
#endif
};