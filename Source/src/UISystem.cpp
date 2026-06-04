/*!
    \file UISystem.cpp
    \brief Implementation of UISystem

    Subscribes to EventCardHovered and EventSnatchCardHovered,
    forwarding each index to \c Board::onCardHovered() and
    \c Board::onSnatchHovered() respectively.
*/

#include "UISystem.h"
#include "Game.h"
#include "Board.h"

//! Stores a reference to \a board for use in event callbacks
UISystem::UISystem(Board& board) : m_board(board) {}

//! Subscribes to EventCardHovered and EventSnatchCardHovered.
void UISystem::bindEvents(Game& game) {
    game.subscribe<EventCardHovered>([&](const EventCardHovered& e) {
        game.getBoard().onCardHovered(e.index);
    });

    game.subscribe<EventSnatchCardHovered>([&](const EventSnatchCardHovered& e) {
        m_board.onSnatchHovered(e.index);
    });
}