#include "AudioSystem.h"
#include "Game.h"
#include <SDL3/SDL.h>

AudioSystem::AudioSystem() {
	if (!MIX_Init()) {
		SDL_Log("AudioSystem: MIX_Init failed: %s", SDL_GetError());
		return;
	}

	m_mixerSFX = MIX_CreateMixerDevice(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		nullptr
	);
	m_mixerUI = MIX_CreateMixerDevice(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		nullptr
	);

	if (!m_mixerSFX) {
		SDL_Log("Cannot create mixerSFX: %s", SDL_GetError());
		return;
	}

	if (!m_mixerUI) {
		SDL_Log("Cannot create mixerUI: %s", SDL_GetError());
		return;
	}
	else {
		MIX_SetMixerGain(m_mixerUI, LOW_VOLUME);
	}
}

AudioSystem::~AudioSystem() {
	for (auto& [id, entry] : m_sounds)
		MIX_DestroyAudio(entry.audio);
	m_sounds.clear();

	m_mixerUI = nullptr;
	m_mixerSFX = nullptr;

	MIX_Quit();
}

void AudioSystem::load(const std::string& id, const std::string& path, MIX_Mixer* mixer) {
	if (!mixer) return;

	MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), false);
	if (!audio) {
		SDL_Log("AudioSystem: cannot load: %s - %s", path.c_str(), SDL_GetError());
		return;
	}
	m_sounds[id] = { audio, mixer };
}

void AudioSystem::play(const std::string& id) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end()) return;

	if (!MIX_PlayAudio(it->second.mixer, it->second.audio))
		SDL_Log("AudioSystem: MIX_PlayAudio failed: %s", SDL_GetError());
}

void AudioSystem::init() {
	// UI
	load("hover", "assets/sounds/hover.wav", m_mixerUI);
	load("turn_player", "assets/sounds/turn_player.wav", m_mixerUI);
	load("turn_ai", "assets/sounds/turn_ai.wav", m_mixerUI);

	// SFX
	load("card_number_1", "assets/sounds/card_number_1.wav", m_mixerSFX);
	load("card_number_2", "assets/sounds/card_number_2.wav", m_mixerSFX);
	load("card_number_3", "assets/sounds/card_number_3.wav", m_mixerSFX);
	load("card_number_4", "assets/sounds/card_number_4.wav", m_mixerSFX);
	load("card_number_5", "assets/sounds/card_number_5.wav", m_mixerSFX);
	load("card_number_6", "assets/sounds/card_number_6.wav", m_mixerSFX);
	load("card_number_7", "assets/sounds/card_number_7.wav", m_mixerSFX);
	load("card_strike", "assets/sounds/card_strike.wav", m_mixerSFX);
	load("card_flip", "assets/sounds/card_flip.wav", m_mixerSFX);
	load("card_snatch", "assets/sounds/card_snatch.wav", m_mixerSFX);
	load("card_double", "assets/sounds/card_double.wav", m_mixerSFX);
	load("win", "assets/sounds/win.wav", m_mixerSFX);
	load("lose", "assets/sounds/lose.wav", m_mixerSFX);
	load("tie", "assets/sounds/tie.wav", m_mixerSFX);
}

void AudioSystem::bindEvents(Game& game) {
	game.subscribe<EventCardHovered>([&](const EventCardHovered& e) {
		onCardHovered(e);
	});

	game.subscribe<EventCardPlayed>([&](const EventCardPlayed& e) {
		onCardPlayed(e);
	});

	game.subscribe<EventGameOver>([&](const EventGameOver& e) {
		onGameOver(e);
	});

	game.subscribe<EventRoundTied>([&](const EventRoundTied& e) {
		onRoundTied(e);
	});

	game.subscribe<EventTurnChanged>([&](const EventTurnChanged& e) {
		onTurnChanged(e);
	});
}

void AudioSystem::onCardHovered(const EventCardHovered& e) {
	if (e.index != -1)
		play("hover");
}

void AudioSystem::onCardPlayed(const EventCardPlayed& e) {
	if (!e.byPlayer) return;
	switch (e.card.type) {
		case CardType::Number: {
			switch (e.card.value) {
				case 1: play("card_number_1"); break;
				case 2: play("card_number_2"); break;
				case 3: play("card_number_3"); break;
				case 4: play("card_number_4"); break;
				case 5: play("card_number_5"); break;
				case 6: play("card_number_6"); break;
				case 7: play("card_number_7"); break;
				default: break;
			}
			break;
		}
		case CardType::Strike:
			play("card_strike"); break;
		case CardType::Flip:
			play("card_flip"); break;
		case CardType::Snatch:
			play("card_snatch"); break;
		case CardType::Double:
			play("card_double"); break;
		default: break;
	}
}

void AudioSystem::onGameOver(const EventGameOver& e) {
	play(e.playerWon ? "win" : "lose");
}

void AudioSystem::onRoundTied(const EventRoundTied& e) {
	play("tie");
}

void AudioSystem::onTurnChanged(const EventTurnChanged& e) {
	play(e.isPlayerTurn ? "turn_player" : "turn_ai");
}