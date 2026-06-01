#include "Layout.h"
#include <algorithm>

namespace Layout {
	
	float WIN_W = BASE_W;
	float WIN_H = BASE_H;
	float safeScale = 1.f;

	float scF(float v) { return v * safeScale; }
	int scFont(float baseSize) {
		float scaleX = WIN_W / BASE_W;
		float scaleY = WIN_H / BASE_H;
		float avgScale = (scaleX + scaleY) * 0.5f;

		return static_cast<int>(baseSize * avgScale);
	}

	float fromLeft(float baseVal) { return baseVal * safeScale; }
	float fromRight(float baseVal) { return WIN_W - scF(BASE_W - baseVal); }
	float fromTop(float baseVal) { return baseVal * safeScale; }
	float fromBottom(float baseVal) { return WIN_H - scF(BASE_H - baseVal); }
	float fromCenterX(float baseVal) { return (WIN_W * 0.5f) + scF(baseVal - (BASE_W * 0.5f)); }
	float fromCenterY(float baseVal) { return (WIN_H * 0.5f) + scF(baseVal - (BASE_H * 0.5f)); }

	float CARD_W() { return scF(156.f); }
	float CARD_H() { return scF(220.f); }
	float CARD_GAP() { return scF(-96.f); }

	float PLY_DECK_X() { return fromLeft(300.f); }
	float PLY_DECK_Y() { return fromBottom(845.f); }

	float OPP_DECK_X() { return fromRight(1320.f); }
	float OPP_DECK_Y() { return fromTop(30.f); }

	float PLY_HAND_X() { return fromCenterX(600.f); }
	float PLY_HAND_Y() { return fromBottom(883.f); }

	float OPP_HAND_X() { return fromCenterX(300.f); }
	float OPP_HAND_Y() { return fromTop(36.f); }

	float HAND_CARD_W() { return scF(116.f); }
	float HAND_CARD_H() { return scF(162.f); }
	float HAND_GAP() { return scF(-32.f); }

	float PLY_FIELD_X() { return fromCenterX(1051.f); }
	float PLY_FIELD_Y() { return fromCenterY(565.f); }

	float OPP_FIELD_X() { return fromCenterX(720.f); }
	float OPP_FIELD_Y() { return fromCenterY(295.f); }

	float SCORE_X() { return fromCenterX(895.f); }
	float SCORE_Y() { return fromCenterY(415.f); }
	float SCORE_W() { return scF(139.f); }
	float SCORE_H() { return scF(230.f); }

	float DIVIDER_Y() { return fromCenterY(540.f); }

	void update(SDL_Window* window) {
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		WIN_W = static_cast<float>(w);
		WIN_H = static_cast<float>(h);

		safeScale = WIN_H / BASE_H;
	}
}