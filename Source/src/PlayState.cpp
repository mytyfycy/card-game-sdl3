/*!
	\file PlayState.cpp
	\brief Implementation of PlayState
*/

#include "PlayState.h"
#include "DrawUtil.h"

//! Stores \a renderer and \a difficulty; systems are not created until \c onEnter()
PlayState::PlayState(SDL_Renderer* renderer, Difficulty difficulty)
	: m_renderer(renderer), m_difficulty(difficulty) {}

/*!
	Constructs \c Game, then registers the Board's TextureManager and TextRenderer
	with the \c draw namespace. Constructs \c AudioSystem and \c UISystem, calls
	their \c init() and \c bindEvents() methods, and in debug builds constructs
	and binds \c DebugSystem.
*/
void PlayState::onEnter() {
	m_game = std::make_unique<Game>(m_renderer, m_difficulty);

	draw::setTextureManager(m_game->getBoard().getTextureManager());
	draw::setTextRenderer(m_game->getBoard().getTextRenderer());

	m_audio = std::make_unique<AudioSystem>();
	m_ui = std::make_unique<UISystem>(m_game->getBoard());

	m_audio->init(*m_game);
	m_audio->bindEvents(*m_game);
	m_ui->bindEvents(*m_game);

#ifdef _DEBUG
	m_debug = std::make_unique<DebugSystem>();
	m_debug->bindEvents(*m_game);
#endif
}

/*!
	Explicitly resets all systems in reverse dependency order:
	\c DebugSystem, \c UISystem, \c AudioSystem, \c Game.
*/
void PlayState::onExit() {
#ifdef _DEBUG
	m_debug.reset();
#endif
	m_ui.reset();
	m_audio.reset();
	m_game.reset();
}

//! Forwards to \c m_game->handleEvent()
void PlayState::handleEvent(const SDL_Event& e) {
	m_game->handleEvent(e);
}

//! Forwards to \c m_game->update()
void PlayState::update() {
	m_game->update();
}

//! Forwards to \c m_game->render()
void PlayState::render() {
	m_game->render();
}