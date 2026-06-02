#include "MainMenu.h"
#include <SDL3/SDL.h>
#include "Layout.h"
#include "DrawUtil.h"
#include "MenuEvents.h"
#include "PlayState.h"

MainMenu::MainMenu(SDL_Renderer* renderer, StateManager* stateManager)
	: m_renderer(renderer),
	m_text(renderer, "assets/fonts/OpenSans.ttf"),
	m_textures(renderer),
	m_stateManager(stateManager) {}

void MainMenu::render() {
	draw::drawBackground("assets/textures/main_menu.png");
	drawTitle();

	for (size_t i = 0; i < m_buttonLabels.size(); ++i) {
		SDL_FRect rect = getButtonRect(static_cast<int>(i));

		if (m_hoveredOption == static_cast<int>(i))
			draw::drawGlow(rect.x, rect.y, rect.w, rect.h);

		drawButton(m_buttonLabels[i], rect);
	}
}

void MainMenu::update() {

}

void MainMenu::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_EVENT_MOUSE_MOTION) {
		int currentHover = -1;

		SDL_FPoint mousePos = { e.motion.x, e.motion.y };

		for (int i = 0; i < 4; ++i) {
			SDL_FRect rect = getButtonRect(i);

			if (SDL_PointInRectFloat(&mousePos, &rect)) {
				currentHover = i;
				break;
			}
		}

		if (currentHover != m_hoveredOption) {
			m_hoveredOption = currentHover;
			EventButtonHovered ev;
			ev.index = currentHover;
			m_dispatcher.emit(ev);
		}
	}

	if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && e.button.button == SDL_BUTTON_LEFT) {
		SDL_FPoint mousePos = { e.button.x, e.button.y };

		for (int i = 0; i < 4; ++i) {
			SDL_FRect rect = getButtonRect(i);

			if (SDL_PointInRectFloat(&mousePos, &rect)) {
				handleButtonClick(i);
				break;
			}
		}
	}
}

void MainMenu::drawTitle() {
	float centerX = Layout::WIN_W * 0.5f;
	float centerY = Layout::WIN_H * 0.25f;
	draw::drawText("Card Game", centerX, centerY, Layout::scFont(128.f), { 255,255,255,255 }, TextAlign::Center);
}

void MainMenu::drawButton(const std::string& text, const SDL_FRect& rect) {
	int w = Layout::scF(200.f);
	int h = Layout::scF(50.f);
	draw::setColor(50, 50, 50);
	draw::fillRect(rect.x, rect.y, rect.w, rect.h);

	float centerX = rect.x + (rect.w * 0.5f);
	float centerY = rect.y + (rect.h * 0.5f);
	draw::drawText(text, centerX, centerY, Layout::scFont(32.f), { 0,255,255,255 }, TextAlign::Center);
}

SDL_FRect MainMenu::getButtonRect(int index) {
	float centerX = Layout::WIN_W * 0.5f;
	float centerY = Layout::WIN_H * 0.25f;
	float buttonOffset = 260.f + (index * 120.f);

	float y = centerY + Layout::scF(buttonOffset);
	float w = Layout::scF(200.f);
	float h = Layout::scF(50.f);

	return SDL_FRect{ centerX - (w * 0.5f), y - (h * 0.5f), w, h };
}

void MainMenu::handleButtonClick(int index) {
	switch (index) {
		case 0:
			m_stateManager->replace(std::make_unique<PlayState>(m_renderer, Difficulty::Nightmare));
			break;
		case 1:
			SDL_Log("Rules");
			break;
		case 2:
			m_stateManager->pop();
			break;
		default: break;
	}
}

void MainMenu::bindEvents() {
	m_dispatcher.subscribe<EventButtonHovered>([&](const EventButtonHovered& e) {
		m_hoveredOption = e.index;
	});
}