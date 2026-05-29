#pragma once
#include <string>

enum class CardType { Back, Number, Strike, Flip, Snatch, Double };

struct Card {
	CardType type = CardType::Back;
	int value = 0;
	std::string texturePath = "assets/textures/card_back.png";
};