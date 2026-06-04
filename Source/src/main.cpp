/*!
    \file main.cpp
    \brief Application entry point

    Initializes SDL, SDL_ttf, and SDL_mixer, creates the window and renderer,
    and runs the main loop. The loop polls events, updates and renders the
    active state via \c StateManager, and displays the current FPS in the
    window title. Exits when the window is closed or \c StateManager runs out
    of states.

    Escape key during \c PlayState replaces it with \c MenuState.
    Window resize events trigger \c Layout::update() to keep all scaled
    coordinates current.
*/

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "StateManager.h"
#include "PlayState.h"
#include "MenuState.h"
#include "Layout.h"
#include <string>
#include <SDL3_ttf/SDL_ttf.h>
#include "DrawUtil.h"

int main(int argc, char* argv[])
{
    // Initialize SDL video, audio, font, and mixer subsystems
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
	
    if (!MIX_Init()) {
        SDL_Log("AudioSystem: MIX_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Create a resizable 1280x720 window with vsync enabled
    SDL_CreateWindowAndRenderer("Card Game", 1280, 720, SDL_WINDOW_RESIZABLE, &window, &renderer);
    Layout::update(window);
    SDL_SetRenderVSync(renderer, 1);

    draw::setRenderer(renderer);

    {
        StateManager states;
        states.push(std::make_unique<MenuState>(renderer, &states));

        bool isRunning = true;
        SDL_Event e;

        do {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) isRunning = false;

                // Escape during gameplay returns to the main menu
                if (e.type == SDL_EVENT_KEY_UP &&
                    e.key.key == SDLK_ESCAPE &&
                    states.isCurrentState<PlayState>())
                    states.replace(std::make_unique<MenuState>(renderer, &states));

                // Keep layout coordinates in sync with the new window size
                if (e.type == SDL_EVENT_WINDOW_RESIZED)
                    Layout::update(window);

                states.handleEvent(e);
            }

            states.update();

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

#ifdef _DEBUG
            uint64_t start = SDL_GetTicksNS();
#endif

            states.render();
            SDL_RenderPresent(renderer);

#ifdef _DEBUG
            uint64_t end = SDL_GetTicksNS();
            uint64_t frameTime = end - start;

            double fps = 1'000'000'000.0 / frameTime;
            SDL_SetWindowTitle(window, std::to_string(fps).c_str());
#endif

        } while (isRunning && !states.isEmpty());
    }

    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
}