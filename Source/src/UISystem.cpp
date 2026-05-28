#include "UISystem.h"
#include "Game.h"
#include "Board.h"

UISystem::UISystem(Board& board) : m_board(board) {}

void UISystem::bindEvents(Game& game) {
    game.subscribe<EventCardHovered>([&](const EventCardHovered& e) {
        game.getBoard().onCardHovered(e.index);
    });

    game.subscribe<EventSnatchCardHovered>([&](const EventSnatchCardHovered& e) {
        m_board.onSnatchHovered(e.index);
    });
}