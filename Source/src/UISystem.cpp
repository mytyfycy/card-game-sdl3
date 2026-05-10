#include "UISystem.h"
#include "Game.h"
#include "Board.h"

UISystem::UISystem(Board& board) : m_board(board) {}

void UISystem::bindEvents(Game& game) {
    game.subscribe<EventCardHovered>([&game](const EventCardHovered& e) {
        game.getBoard().onCardHovered(e.index);
    });
}

void UISystem::onCardHovered(const EventCardHovered& e) {
    m_board.onCardHovered(e.index);
}