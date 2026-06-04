#pragma once
#include "GameEvents.h"

class Game;

/*!
    \class DebugSystem
    \brief Logs game events to the SDL console output

    Subscribes to key game events and prints their outcomes via \c SDL_Log.
    Intended for development use only.
*/
class DebugSystem {
public:
    /*!
        \brief Subscribes to game events and registers SDL_Log handlers
        \param game Reference to the active Game instance
    */
	void bindEvents(Game& game);
};