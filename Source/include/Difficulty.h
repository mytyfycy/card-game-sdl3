#pragma once

/*!
	\enum Difficulty
	\brief AI difficulty levels available at game start

	\var Difficulty::VeryEasy   AI plays optimally 25% of the time
	\var Difficulty::Easy       AI plays optimally 50% of the time
	\var Difficulty::Normal     AI plays optimally 75% of the time
	\var Difficulty::Hard       AI plays optimally 90% of the time
	\var Difficulty::Nightmare  AI always plays optimally
*/
enum class Difficulty {
	VeryEasy,
	Easy,
	Normal,
	Hard,
	Nightmare
};

/*!
	\brief Returns the optimal-play probability for a given difficulty
	\param d Difficulty level to query
	\return Value in range [0.0, 1.0] representing the chance the AI plays optimally
*/
inline float difficultyToChance(Difficulty d) {
	switch (d) {
	case Difficulty::VeryEasy: return 0.25f;
	case Difficulty::Easy: return 0.5f;
	case Difficulty::Normal: return 0.75f;
	case Difficulty::Hard: return 0.9f;
	case Difficulty::Nightmare: return 1.0f;
	default: return 0.75f;
	}
}