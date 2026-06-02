#include "MenuState.h"
#include <memory>
#include "DrawUtil.h"

MenuState::MenuState(SDL_Renderer* renderer, StateManager* stateManager) : m_renderer(renderer), m_stateManager(stateManager) {}

void MenuState::onEnter() {
	m_menu = std::make_unique<MainMenu>(m_renderer, m_stateManager);

	draw::setTextureManager(m_menu->getTextureManager());
	draw::setTextRenderer(m_menu->getTextRenderer());

	m_audio = std::make_unique<AudioSystem>();
	m_audio->init(*m_menu);
	m_audio->bindEvents(*m_menu);
}

void MenuState::onExit() {
	m_menu.reset();
}

void MenuState::handleEvent(const SDL_Event& e) {
	m_menu->handleEvent(e);
}

void MenuState::update() {
	m_menu->update();
}

void MenuState::render() {
	m_menu->render();
}