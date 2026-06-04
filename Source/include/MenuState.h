#pragma once
#include "GameStateBase.h"
#include "MainMenu.h"
#include <memory>
#include "SDL3/SDL.h"
#include "AudioSystem.h"

/*!
	\class MenuState
	\brief GameStateBase implementation for the main menu screen

	Owns \c MainMenu and \c AudioSystem. Registers draw utilities
	and binds audio events in \c onEnter(); tears down on \c onExit().
*/
class MenuState : public GameStateBase {
public:
	/*!
		\brief Stores renderer and state manager for use during \c onEnter()
		\param renderer     SDL renderer passed to \c MainMenu
		\param stateManager StateManager passed to \c MainMenu for state transitions
	*/
	MenuState(SDL_Renderer* renderer, StateManager* stateManager);

	//! Creates \c MainMenu and \c AudioSystem, registers draw utilities, and binds audio events
	void onEnter() override;

	//! Destroys \c MainMenu and \c AudioSystem
	void onExit() override;

	//! Forwards the event to \c MainMenu::handleEvent()
	void handleEvent(const SDL_Event& e) override;

	//! Forwards to \c MainMenu::update()
	void update() override;

	//! Forwards to \c MainMenu::render()
	void render() override;

private:
	SDL_Renderer* m_renderer; //!< SDL renderer passed to MainMenu on enter
	StateManager* m_stateManager; //!< StateManager passed to MainMenu for transitions

	std::unique_ptr<MainMenu> m_menu; //!< Main menu UI and logic
	std::unique_ptr<AudioSystem> m_audio; //!< Sound playback for menu interactions
};