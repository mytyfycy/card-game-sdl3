#pragma once
#include <string>

/*!
    \enum CardType
    \brief Identifies the type of a card

    \var CardType::Back    Face-down, used for opponent's cards
    \var CardType::Number  Standard numbered card with a value from 1 to 7
    \var CardType::Strike  Removes the last card played by the opponent
    \var CardType::Flip    Swaps all cards currently on both sides of the field
    \var CardType::Snatch  Discards a random card from the opponent's hand
    \var CardType::Double  Doubles the player's current field total
*/
enum class CardType { Back, Number, Strike, Flip, Snatch, Double };

/*!
    \struct Card
    \brief Represents a single card in the game

    Holds the card's type, numeric value (relevant for Number cards),
    and the path to its face texture.
*/
struct Card {
	CardType type = CardType::Back; //!< Card type determining its gameplay role
	int value = 0; //!< Numeric value; meaningful only for Number cards
	std::string texturePath = "assets/textures/card_back.png"; //!< Path to the card's face texture
};