#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Game.h"

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer("Card Game", 1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetRenderVSync(renderer, 1);

    Game game(renderer);

    // Subskrypcje eventow
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

    game.subscribe<EventCardHovered>([&game](const EventCardHovered& e) {
        game.getBoard().onCardHovered(e.index);
    });

    bool isRunning = true;
    SDL_Event e;

    while (isRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) isRunning = false;
            if (e.type == SDL_EVENT_KEY_DOWN &&
                e.key.key == SDLK_ESCAPE) isRunning = false;
            game.handleEvent(e);
        }

        game.update();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        game.render();
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}