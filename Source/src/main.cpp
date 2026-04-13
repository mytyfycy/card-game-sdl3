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