#pragma once
#include "BaseEvent.h"
#include "Card.h"

/*!
	\struct EventGameOver
	\brief Emitted when the game ends
*/
struct EventGameOver : EventBase {
	bool playerWon; //!< \c true if the player won, \c false if they lost
};

/*!
	\struct EventTurnChanged
	\brief Emitted when the active turn switches between player and AI
*/
struct EventTurnChanged : EventBase {
	bool isPlayerTurn; //!< \c true if it is now the player's turn
};

/*!
	\struct EventRoundTied
	\brief Emitted when both players reach an equal score and the field is cleared
*/
struct EventRoundTied : EventBase {};

/*!
	\struct EventCardPlayed
	\brief Emitted when a card is played onto the field
*/
struct EventCardPlayed : EventBase {
	Card card; //!< The card that was played
	bool byPlayer; //!< \c true if played by the player, \c false if by the AI
};

/*!
	\struct EventCardRemoved
	\brief Emitted when a card is removed from a field by a Strike
*/
struct EventCardRemoved : EventBase {
	Card card; //!< The card that was removed
	bool fromPlayer; //!< \c true if removed from the player's field, \c false if from the AI's
};

/*!
	\struct EventSnatchCardHovered
	\brief Emitted when the mouse moves over an opponent's hand card during Snatch target selection
*/
struct EventSnatchCardHovered : EventBase {
	int index; //!< Hovered card index, or -1 if none
};

/*!
	\struct EventCardHovered
	\brief Emitted when the mouse moves over a card in the player's hand
*/
struct EventCardHovered : EventBase {
	int index; //!< Hovered card index, or -1 if none
};

/*!
	\struct EventSnatchTargetRequired
	\brief Emitted when a Snatch card is played and the player must select a target
*/
struct EventSnatchTargetRequired : EventBase {
	size_t opponentHandSize; //!< Number of cards available to snatch from the opponent's hand
};

/*!
	\struct EventSnatchResolved
	\brief Emitted when a Snatch target has been selected and the card removed
*/
struct EventSnatchResolved : EventBase {
	Card removedCard; //!< The card that was removed from the opponent's hand
};