#pragma once
#include <string>

enum class CardType { Number, Strike, Flip, Blast, Force };

struct Card {
	CardType type;
	int value;
	std::string texturePath;
};