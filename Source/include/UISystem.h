#pragma once
#include "GameEvents.h"

class Game;
class Board;

/*!
    \class UISystem
    \brief Bridges game events to Board hover state

    Subscribes to hover events and forwards the relevant index
    to the appropriate \c Board method so the correct card is
    highlighted each frame.
*/
class UISystem {
public:
    /*!
        \brief Constructs UISystem with a reference to the Board it will update
        \param board Board whose hover state will be driven by game events
    */
	UISystem(Board& board);

    /*!
        \brief Subscribes to hover events from the given Game instance
        \param game Reference to the active Game instance
    */
	void bindEvents(Game& game);

private:
	Board& m_board; //!< Board whose hover state is updated on each event
};