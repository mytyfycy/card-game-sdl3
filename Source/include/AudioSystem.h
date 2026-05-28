#pragma once
#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
#include <string>
#include "GameEvents.h"
#include "Card.h"

class Game;

class AudioSystem {
public:
	static constexpr float MAX_VOLUME = 1.0f;
	static constexpr float MID_VOLUME = 0.66f;
	static constexpr float LOW_VOLUME = 0.05f;

	AudioSystem();
	~AudioSystem();

	void init();
	void bindEvents(Game& game);
	void play(const std::string& id);

private:
	MIX_Mixer* m_mixerSFX = nullptr;
	MIX_Mixer* m_mixerUI = nullptr;

	struct SoundEntry {
		MIX_Audio* audio;
		MIX_Mixer* mixer;
	};
	std::unordered_map<std::string, SoundEntry> m_sounds;

	void load(const std::string& id, const std::string& path, MIX_Mixer* mixer);

	void onCardHovered(const EventCardHovered& e);
	void onCardPlayed(const EventCardPlayed& e);
	void onGameOver(const EventGameOver& e);
	void onRoundTied(const EventRoundTied& e);
	void onTurnChanged(const EventTurnChanged& e);
};