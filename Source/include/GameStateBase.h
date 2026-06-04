#pragma once
#include <SDL3/SDL.h>

class StateManager;

/*!
	\class GameStateBase
	\brief Abstract base class for all application states

	Each concrete state (e.g. \c PlayState, \c MainMenu) implements the
	three pure virtual methods. \c m_manager is injected by \c StateManager
	before \c onEnter() is called, giving states access to push, pop,
	or replace themselves.
*/
class  GameStateBase {
public:
	virtual ~GameStateBase() = default;

	//! Called by \c StateManager when this state becomes active
	virtual void onEnter() {}

	//! Called by \c StateManager just before this state is deactivated or destroyed
	virtual void onExit() {}

	//! Processes a single SDL event
	virtual void handleEvent(const SDL_Event& e) = 0;

	//! Advances state logic by one frame
	virtual void update() = 0;

	//! Draws the current frame
	virtual void render() = 0;

protected:
	//! Pointer to the owning StateManager; set before \c onEnter() by \c StateManager
	StateManager* m_manager = nullptr;

	friend class StateManager;
};