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
	m_stateManager(stateManager),
	m_buttonLabels{ "Play", "Tutorial", "Quit" },
	m_difficultyLabels{ "Very Easy", "Easy", "Normal", "Hard", "Nightmare" } {

	m_buttonLabelsSize = m_buttonLabels.size();
	m_difficultyLabelsSize = m_difficultyLabels.size();
	initTutorial();
}

void MainMenu::render() {
	draw::drawBackground("assets/textures/main_menu.png");
	drawTitle();

	for (size_t i = 0; i < m_buttonLabelsSize; ++i) {
		SDL_FRect rect = getButtonRect(static_cast<int>(i));

		if (m_currentSelectState == SelectState::Main && m_hoveredButton == static_cast<int>(i))
			draw::drawGlow(rect.x, rect.y, rect.w, rect.h);

		drawButton(m_buttonLabels[i], rect);
	}

	switch (m_currentSelectState) {
		case SelectState::DifficultySelect: {
			SDL_FRect overlayRect = getOverlayRect();
			draw::drawOverlay(overlayRect.x, overlayRect.y, overlayRect.w, overlayRect.h);

			draw::drawText("Select Difficulty", overlayRect.x + overlayRect.w * 0.5f, overlayRect.y + Layout::scF(100.f), Layout::scFont(96.f), { 255,255,255,255 }, TextAlign::Center);

			for (int i = 0; i < m_difficultyLabelsSize; ++i) {
				SDL_FRect buttonRect = getOverlayButtonRect(static_cast<int>(i), static_cast<int>(m_difficultyLabelsSize));

				if (m_hoveredOverlayButton == static_cast<int>(i))
					draw::drawGlow(buttonRect.x, buttonRect.y, buttonRect.w, buttonRect.h);

				drawButton(m_difficultyLabels[i], buttonRect);
			}
		}
		break;
	
		case SelectState::Tutorial: {
			SDL_FRect overlayRect = getOverlayRect();

			draw::drawOverlay(overlayRect.x, overlayRect.y, overlayRect.w, overlayRect.h);
			draw::drawText("Tutorial", overlayRect.x + overlayRect.w * 0.5f, overlayRect.y + Layout::scF(100.f), Layout::scFont(96.f), { 255,255,255,255 }, TextAlign::Center);

			draw::drawText(m_tutorialPages[m_currentTutorialPage], overlayRect.x + overlayRect.w * 0.5f + Layout::scF(10.f), overlayRect.y + overlayRect.h * 0.5f + Layout::scF(2.5f), Layout::scFont(48.f), {125,230,125,255}, TextAlign::Center, static_cast<int>(overlayRect.w * 0.75f - Layout::scF(10.f)));

			if (m_currentTutorialPage > 0) {
				SDL_FRect leftArrow = getLeftArrowRect();
				if (m_hoveredTutorialArrow == ArrowState::Left)
					draw::drawGlow(leftArrow.x, leftArrow.y, leftArrow.w, leftArrow.h);
				drawButton("<", leftArrow);
			}

			if (m_currentTutorialPage < static_cast<int>(m_tutorialPages.size()) - 1) {
				SDL_FRect rightArrow = getRightArrowRect();
				if (m_hoveredTutorialArrow == ArrowState::Right)
					draw::drawGlow(rightArrow.x, rightArrow.y, rightArrow.w, rightArrow.h);
				drawButton(">", rightArrow);
			}

			std::string pageCounter = std::to_string(m_currentTutorialPage + 1) + " / " + std::to_string(m_tutorialPages.size());
			draw::drawText(pageCounter, overlayRect.x + overlayRect.w * 0.5f, overlayRect.y + overlayRect.h - Layout::scF(45.f), Layout::scFont(20.f), { 150, 150, 150, 255 }, TextAlign::Center);
		}
		break;
	}
}

void MainMenu::update() {

}

void MainMenu::handleEvent(const SDL_Event& e) {
	SDL_FPoint mousePos = { e.motion.x, e.motion.y };

	if (e.type == SDL_EVENT_MOUSE_MOTION) {

		switch (m_currentSelectState) {
		case SelectState::Main: {
			int currentHover = -1;

			for (int i = 0; i < m_buttonLabelsSize; ++i) {
				SDL_FRect rect = getButtonRect(i);

				if (SDL_PointInRectFloat(&mousePos, &rect)) {
					currentHover = i;
					break;
				}
			}

			if (m_hoveredButton != currentHover) {
				EventButtonHovered ev;
				ev.index = currentHover;
				m_dispatcher.emit(ev);
			}

			m_hoveredButton = currentHover;

			break;
		}
		case SelectState::DifficultySelect: {
			int currentHover = -1;
			for (size_t i = 0; i < m_difficultyLabelsSize; ++i) {
				SDL_FRect rect = getOverlayButtonRect(static_cast<int>(i), static_cast<int>(m_difficultyLabelsSize));
				if (SDL_PointInRectFloat(&mousePos, &rect)) {
					currentHover = static_cast<int>(i);
					break;
				}
			}

			if (m_hoveredOverlayButton != currentHover) {
				EventButtonHovered ev;
				ev.index = currentHover;
				m_dispatcher.emit(ev);
			}

			m_hoveredOverlayButton = currentHover;
			break;
		}
		case SelectState::Tutorial: {
			ArrowState currentArrowHover = ArrowState::None;
			SDL_FRect left = getLeftArrowRect();
			SDL_FRect right = getRightArrowRect();

			if (m_currentTutorialPage > 0 && SDL_PointInRectFloat(&mousePos, &left))
				currentArrowHover = ArrowState::Left;
			else if (m_currentTutorialPage < static_cast<int>(m_tutorialPages.size()) - 1
				&& SDL_PointInRectFloat(&mousePos, &right))
				currentArrowHover = ArrowState::Right;

			if (m_hoveredTutorialArrow != currentArrowHover 
				&& currentArrowHover != ArrowState::None) {
				EventButtonHovered ev;
				ev.index = static_cast<int>(currentArrowHover);
				m_dispatcher.emit(ev);
			}

			m_hoveredTutorialArrow = currentArrowHover;
			break;
		}
		}
	}

	if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && e.button.button == SDL_BUTTON_LEFT) {
		SDL_FPoint mousePos = { e.button.x, e.button.y };

		if (m_currentSelectState == SelectState::Main) {
			for (int i = 0; i < m_buttonLabelsSize; ++i) {
				SDL_FRect rect = getButtonRect(i);
				if (SDL_PointInRectFloat(&mousePos, &rect)) {
					handleButtonClick(i);
					break;
				}
			}
		}
		else {
			SDL_FRect overlayRect = getOverlayRect();

			if (!SDL_PointInRectFloat(&mousePos, &overlayRect)) {
				m_currentSelectState = SelectState::Main;
				m_hoveredOverlayButton = -1;
				m_currentTutorialPage = 0;
				return;
			}

			switch (m_currentSelectState) {
			case SelectState::DifficultySelect: {
				for (size_t i = 0; i < m_difficultyLabelsSize; ++i) {
					SDL_FRect buttonRect = getOverlayButtonRect(static_cast<int>(i), static_cast<int>(m_difficultyLabelsSize));
					if (SDL_PointInRectFloat(&mousePos, &buttonRect)) {
						Difficulty chosenDiff = static_cast<Difficulty>(i);
						m_stateManager->replace(std::make_unique<PlayState>(m_renderer, chosenDiff));
						break;
					}
				}
				break;
			}
			case SelectState::Tutorial: {
				SDL_FRect left = getLeftArrowRect();
				SDL_FRect right = getRightArrowRect();

				if (m_currentTutorialPage > 0 && SDL_PointInRectFloat(&mousePos, &left)) {
					m_currentTutorialPage--;
					m_hoveredTutorialArrow = ArrowState::None;
				}
				else if (m_currentTutorialPage < static_cast<int>(m_tutorialPages.size()) - 1
					&& SDL_PointInRectFloat(&mousePos, &right)) {
					m_currentTutorialPage++;
					m_hoveredTutorialArrow = ArrowState::None;
				}
				break;
			}
			}
		}
	}
}

void MainMenu::drawTitle() {
	float centerX = Layout::WIN_W * 0.5f;
	float centerY = Layout::WIN_H * 0.25f;
	draw::drawText("Card Game", centerX, centerY, Layout::scFont(128.f), { 255,255,255,255 }, TextAlign::Center);
}

void MainMenu::drawDifficultySelection() {
	float centerX = Layout::WIN_W * 0.65f;
	float centerY = Layout::WIN_H * 0.75f;
	
	float w = Layout::scF(Layout::WIN_W * 0.5f);
	float h = Layout::scF(Layout::WIN_H * 0.5f);
	draw::drawOverlay(centerX - w * 0.5f, centerY + h * 0.5f, w, h);
}

SDL_FRect MainMenu::getOverlayRect() {
	float w = Layout::WIN_W * 0.65f;
	float h = Layout::WIN_H * 0.75f;

	float x = (Layout::WIN_W * 0.5f) - (w * 0.5f);
	float y = (Layout::WIN_H * 0.5f) - (h * 0.5f);

	return SDL_FRect{ x,y,w,h };
}

SDL_FRect MainMenu::getOverlayButtonRect(int index, int totalButtons)
{
	SDL_FRect overlay = getOverlayRect();

	float buttonW = overlay.w * 0.8f;
	float buttonH = Layout::scF(50.f);

	float startY = overlay.y + (Layout::WIN_H * 0.25f);
	float spacing = Layout::scF(60.f);

	float x = overlay.x + (overlay.w * 0.5f) - (buttonW * 0.5f);
	float y = startY + index * (buttonH + spacing);

	return SDL_FRect{ x, y, buttonW, buttonH };
}

void MainMenu::drawButton(const std::string& text, const SDL_FRect& rect) {
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
			m_currentSelectState = SelectState::DifficultySelect;
			break;
		case 1:
			m_currentSelectState = SelectState::Tutorial;
			break;
		case 2:
			m_stateManager->pop();
			break;
		default: break;
	}
}

void MainMenu::bindEvents() {
	m_dispatcher.subscribe<EventButtonHovered>([&](const EventButtonHovered& e) {
		m_hoveredButton = e.index;
	});
}

void MainMenu::initTutorial() {
	m_tutorialPages.push_back("Card Game is played between two players who take turns placing numbered cards on the field, competing to achieve the higher total value.");
	m_tutorialPages.push_back("At the start of the game, both players are dealt their hands. Each player then draws a card from the deck and places it onto the field automatically.");
	m_tutorialPages.push_back("The player who draws the lower-valued card takes the next turn and must play a card from their hand onto the field.");
	m_tutorialPages.push_back("The opposing player must then play a card with a value high enough to surpass their opponent's current total.");
	m_tutorialPages.push_back("Play continues in this manner until one player can no longer exceed their opponent's total or runs out of cards.");
	m_tutorialPages.push_back("If both players reach an equal total at any point, the field is cleared, and both players draw and play new cards. Play continues with their existing hands.");
	m_tutorialPages.push_back("There are five types of cards in Card Game.");
	m_tutorialPages.push_back("Normal Cards are the most common type and are numbered from 1 to 7.");
	m_tutorialPages.push_back("Strike Cards remove the last card placed on the field by your opponent.");
	m_tutorialPages.push_back("As a special rule, a Normal Card with a value of 1 may be used to restore a card removed by a Strike Card.");
	m_tutorialPages.push_back("Flip Cards switch the positions of all cards currently on the field.");
	m_tutorialPages.push_back("Snatch Cards allow you to discard one card from your opponent's hand. However, you may not view their hand before making your selection.");
	m_tutorialPages.push_back("Double Cards double your current total score.");
	m_tutorialPages.push_back("These cards are highly powerful but require careful timing to maximize their effectiveness.");
	m_tutorialPages.push_back("Be aware that using any special card as your final move is considered a foul and results in an automatic loss.");
}

SDL_FRect MainMenu::getLeftArrowRect()
{
	SDL_FRect overlay = getOverlayRect();
	float size = Layout::scF(50.f);

	float x = overlay.x + Layout::scF(30.f);
	float y = overlay.y + overlay.h - size - Layout::scF(30.f);
	return SDL_FRect{ x,y,size,size };
}

SDL_FRect MainMenu::getRightArrowRect()
{
	SDL_FRect overlay = getOverlayRect();
	float size = Layout::scF(50.f);
	float x = overlay.x + overlay.w - size - Layout::scF(30.f);
	float y = overlay.y + overlay.h - size - Layout::scF(30.f);
	return SDL_FRect{ x,y,size,size };
}
