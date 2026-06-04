/*!
	\file MenuState.cpp
	\brief Implementation of MenuState
*/

#include "MenuState.h"
#include <memory>
#include "DrawUtil.h"

//! Stores \a renderer and \a stateManager; systems are not created until \c onEnter()
MenuState::MenuState(SDL_Renderer* renderer, StateManager* stateManager) 
	: m_renderer(renderer), m_stateManager(stateManager) {}

/*!
	Constructs \c MainMenu, then registers its TextureManager and TextRenderer
	with the \c draw namespace. Constructs \c AudioSystem and calls
	\c init() and \c bindEvents() with the menu instance.
*/
void MenuState::onEnter() {
	m_menu = std::make_unique<MainMenu>(m_renderer, m_stateManager);

	draw::setTextureManager(m_menu->getTextureManager());
	draw::setTextRenderer(m_menu->getTextRenderer());

	m_audio = std::make_unique<AudioSystem>();
	m_audio->init(*m_menu);
	m_audio->bindEvents(*m_menu);
}

//! Resets \c MainMenu and \c AudioSystem
void MenuState::onExit() {
	m_menu.reset();
}

//! Forwards to \c m_menu->handleEvent()
void MenuState::handleEvent(const SDL_Event& e) {
	m_menu->handleEvent(e);
}

//! Forwards to \c m_menu->update()
void MenuState::update() {
	m_menu->update();
}

//! Forwards to \c m_menu->render()
void MenuState::render() {
	m_menu->render();
}