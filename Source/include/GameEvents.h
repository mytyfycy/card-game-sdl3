#pragma once
#include "Card.h"

// Bazowy event
struct GameEventBase {
	virtual ~GameEventBase() = default;
};

// Koniec gry
struct EventGameOver : GameEventBase {
	bool playerWon;
};

// Zmiana tury
struct EventTurnChanged : GameEventBase {
	bool isPlayerTurn;
};

// Remis
struct EventRoundTied : GameEventBase {};

// Zagrana karta
struct EventCardPlayed : GameEventBase {
	Card card;
	bool byPlayer;
};

// Karta usunieta
struct EventCardRemoved : GameEventBase {
	Card card;
	bool fromPlayer;
};

// Najechano myszka na karte (podczas uzycia Snatch)
struct EventSnatchCardHovered : GameEventBase {
	int index;
};

// Najechano myszka na karte
struct EventCardHovered : GameEventBase {
	int index; // -1 = brak
};

// Trzeba wybrac karte przy uzyciu Snatch
struct EventSnatchTargetRequired : GameEventBase {
	int opponentHandSize;
};

// Usunieta karta przez Snatch
struct EventSnatchResolved : GameEventBase {
	Card removedCard;
};
