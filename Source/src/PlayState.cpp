#include "PlayState.h"
#include "DrawUtil.h"

PlayState::PlayState(SDL_Renderer* renderer, Difficulty difficulty)
	: m_renderer(renderer), m_difficulty(difficulty) {}

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

void PlayState::onExit() {
	// Unique_ptr same sie sprzataja
	m_debug.reset();
	m_ui.reset();
	m_audio.reset();
	m_game.reset();
}

void PlayState::handleEvent(const SDL_Event& e) {
	m_game->handleEvent(e);
}

void PlayState::update() {
	m_game->update();
}

void PlayState::render() {
	m_game->render();
}