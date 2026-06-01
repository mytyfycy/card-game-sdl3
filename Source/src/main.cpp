#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Game.h"
#include "DebugSystem.h"
#include "UISystem.h"
#include "AudioSystem.h"
#include "Layout.h"

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer("Card Game", 1280, 720, SDL_WINDOW_RESIZABLE, &window, &renderer);
    Layout::update(window);
    SDL_SetRenderVSync(renderer, 1);

    {
        Game game(renderer, Difficulty::Nightmare);

        bool isRunning = true;
        SDL_Event e;

        // Subskrypcje eventow
        AudioSystem audio;
        UISystem ui(game.getBoard());

        audio.init();
        audio.bindEvents(game);
        ui.bindEvents(game);

#ifdef _DEBUG
        DebugSystem debug;
        debug.bindEvents(game);
#endif

        while (isRunning) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) isRunning = false;
                if (e.type == SDL_EVENT_KEY_DOWN &&
                    e.key.key == SDLK_ESCAPE) isRunning = false;

                if (e.type == SDL_EVENT_WINDOW_RESIZED)
                    Layout::update(window);
                game.handleEvent(e);
            }

            game.update();

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            uint64_t start = SDL_GetTicksNS();
            game.render();
            SDL_RenderPresent(renderer);
            uint64_t end = SDL_GetTicksNS();
            uint64_t frameTime = end - start;

            double fps = 1'000'000'000.0 / frameTime;
            SDL_SetWindowTitle(window, std::to_string(fps).c_str());
        }
    }

    SDL_Quit();

    return 0;
}