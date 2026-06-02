#pragma once
#include "BaseEvent.h"
#include "Card.h"

// Koniec gry
struct EventGameOver : EventBase {
	bool playerWon;
};

// Zmiana tury
struct EventTurnChanged : EventBase {
	bool isPlayerTurn;
};

// Remis
struct EventRoundTied : EventBase {};

// Zagrana karta
struct EventCardPlayed : EventBase {
	Card card;
	bool byPlayer;
};

// Karta usunieta
struct EventCardRemoved : EventBase {
	Card card;
	bool fromPlayer;
};

// Najechano myszka na karte (podczas uzycia Snatch)
struct EventSnatchCardHovered : EventBase {
	int index;
};

// Najechano myszka na karte
struct EventCardHovered : EventBase {
	int index; // -1 = brak
};

// Trzeba wybrac karte przy uzyciu Snatch
struct EventSnatchTargetRequired : EventBase {
	size_t opponentHandSize;
};

// Usunieta karta przez Snatch
struct EventSnatchResolved : EventBase {
	Card removedCard;
};