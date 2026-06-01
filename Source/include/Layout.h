#pragma once
#include <SDL3/SDL.h>

namespace Layout {

	inline constexpr float BASE_W = 1920.f;
	inline constexpr float BASE_H = 1080.f;

	extern float WIN_W;
	extern float WIN_H;
	extern float safeScale;

	float scF(float v);
	int scFont(float baseSize);

	float fromLeft(float baseVal);
	float fromRight(float baseVal);
	float fromTop(float baseVal);
	float fromBottom(float baseVal);
	float fromCenterX(float baseVal);
	float fromCenterY(float baseVal);

	float CARD_W();
	float CARD_H();
	float CARD_GAP();

	float PLY_DECK_X();
	float PLY_DECK_Y();

	float OPP_DECK_X();
	float OPP_DECK_Y();

	float PLY_HAND_X();
	float PLY_HAND_Y();

	float OPP_HAND_X();
	float OPP_HAND_Y();

	float HAND_CARD_W();
	float HAND_CARD_H();
	float HAND_GAP();

	float PLY_FIELD_X();
	float PLY_FIELD_Y();

	float OPP_FIELD_X();
	float OPP_FIELD_Y();

	float SCORE_X();
	float SCORE_Y();
	float SCORE_W();
	float SCORE_H();

	float DIVIDER_Y();

	void update(SDL_Window* window);
}