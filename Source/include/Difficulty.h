#pragma once

enum class Difficulty {
	Easy,
	Normal,
	Hard,
	Nightmare
};

inline float difficultyToChance(Difficulty d) {
	switch (d) {
	case Difficulty::Easy: return 0.5f;
	case Difficulty::Normal: return 0.75f;
	case Difficulty::Hard: return 0.9f;
	case Difficulty::Nightmare: return 1.0f;
	default: return 0.75f;
	}
}