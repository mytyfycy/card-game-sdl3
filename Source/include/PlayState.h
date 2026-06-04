#pragma once
#include "GameStateBase.h"
#include "Game.h"
#include "AudioSystem.h"
#include "UISystem.h"
#include "DebugSystem.h"
#include "Difficulty.h"

/*!
	\class PlayState
	\brief GameStateBase implementation for an active game session

	Owns and wires together all systems required during play:
	\c Game, \c AudioSystem, \c UISystem, and (in debug builds) \c DebugSystem.
	Systems are created in \c onEnter() and destroyed in \c onExit().
*/
class PlayState : public GameStateBase {
public:
	/*!
		\brief Stores renderer and difficulty for use during \c onEnter()
		\param renderer   SDL renderer passed to \c Game
		\param difficulty AI difficulty level for the session
	*/
	PlayState(SDL_Renderer* renderer, Difficulty difficulty);

	//! Creates all systems, registers draw utilities, and binds events
	void onEnter() override;

	//! Destroys all systems in reverse dependency order
	void onExit() override;

	//! Forwards the event to \c Game::handleEvent()
	void handleEvent(const SDL_Event& e) override;

	//! Forwards to \c Game::update()
	void update() override;

	//! Forwards to \c Game::render()
	void render() override;

private:
	SDL_Renderer* m_renderer; //!< SDL renderer passed to the Game on enter
	Difficulty m_difficulty; //!< AI difficulty level for this session

	std::unique_ptr<Game> m_game; //!< Core game logic and state
	std::unique_ptr<AudioSystem> m_audio; //!< Sound playback system
	std::unique_ptr<UISystem> m_ui; //!< Hover state bridge between Game and Board
#ifdef _DEBUG
	std::unique_ptr<DebugSystem> m_debug; //!< SDL_Log event logger, debug builds only
#endif
};