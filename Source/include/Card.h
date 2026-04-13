#pragma once
#include <string>

enum class CardType { Number, Bolt, Mirror, Blast, Force };

struct Card {
	CardType type;
	int value;
	std::string texturePath; // TODO
};