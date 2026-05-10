#include "DebugSystem.h"
#include "Game.h"

void DebugSystem::bindEvents(Game& game) {
    game.subscribe<EventGameOver>([](const EventGameOver& e) {
        SDL_Log(e.playerWon ? "Player won" : "Player lost");
        });

    game.subscribe<EventTurnChanged>([](const EventTurnChanged& e) {
        SDL_Log(e.isPlayerTurn ? "Player turn" : "AI turn");
        });

    game.subscribe<EventCardPlayed>([](const EventCardPlayed& e) {
        SDL_Log("Card of type %d with value %d was played by %s",
            static_cast<int>(e.card.type),
            e.card.value,
            e.byPlayer ? "the player" : "AI");
        });

    game.subscribe<EventCardRemoved>([](const EventCardRemoved& e) {
        SDL_Log("Card with value %d was removed from %s",
            e.card.value,
            e.fromPlayer ? "the player" : "AI");
        });

    game.subscribe<EventRoundTied>([](const EventRoundTied&) {
        SDL_Log("Game tied - starting new round");
        });
}