#pragma once
#include <SDL3/SDL.h>

class StateManager;

class  GameStateBase {
public:
	virtual ~GameStateBase() = default;

	virtual void onEnter() {}
	virtual void onExit() {}

	virtual void handleEvent(const SDL_Event& e) = 0;
	virtual void update() = 0;
	virtual void render() = 0;

protected:
	StateManager* m_manager = nullptr;
	friend class StateManager;
};