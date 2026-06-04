/*!
	\file AudioSystem.cpp
	\brief Implementation of AudioSystem

	Handles SDL mixer initialization, asset loading, event subscription,
	and per-event sound dispatch for both game and menu contexts.
*/

#include "AudioSystem.h"
#include "Game.h"
#include "MainMenu.h"
#include <SDL3/SDL.h>

AudioSystem::AudioSystem() {
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
}

/*! Skips loading if \a mixer is null. Logs an SDL error on load failure. */
void AudioSystem::load(const std::string& id, const std::string& path, MIX_Mixer* mixer) {
	if (!mixer) return;

	MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), false);
	if (!audio) {
		SDL_Log("AudioSystem: %s", SDL_GetError());
		return;
	}
	m_sounds[id] = { audio, mixer };
}

/*! Silently ignores unknown identifiers. Logs an SDL error on playback failure. */
void AudioSystem::play(const std::string& id) {
	auto it = m_sounds.find(id);
	if (it == m_sounds.end()) return;

	if (!MIX_PlayAudio(it->second.mixer, it->second.audio))
		SDL_Log("AudioSystem: MIX_PlayAudio failed: %s", SDL_GetError());
}

/*!
	Loads UI sounds (\c hover, turn cues) and SFX (card-type sounds,
	win/lose/tie stings) for use during a game session.
*/
void AudioSystem::init(Game& game) {
	// UI
	load("hover", "assets/sounds/hover.wav", m_mixerUI);
	//load("turn_player", "assets/sounds/turn_player.wav", m_mixerUI); //!< Currently missing a sound file
	//load("turn_ai", "assets/sounds/turn_ai.wav", m_mixerUI); //!< Currently missing a sound file

	// SFX
	load("card_number", "assets/sounds/card_number.wav", m_mixerSFX);
	load("card_strike", "assets/sounds/card_strike.wav", m_mixerSFX);
	load("card_flip", "assets/sounds/card_flip.wav", m_mixerSFX);
	load("card_snatch", "assets/sounds/card_snatch.wav", m_mixerSFX);
	load("card_double", "assets/sounds/card_double.wav", m_mixerSFX);
	//load("win", "assets/sounds/win.wav", m_mixerSFX); //!< Currently missing a sound file
	//load("lose", "assets/sounds/lose.wav", m_mixerSFX); //!< Currently missing a sound file
	//load("tie", "assets/sounds/tie.wav", m_mixerSFX); //!< Currently missing a sound file
}

//! Loads the \c hover UI sound for use in the main menu.
void AudioSystem::init(MainMenu& menu) {
	// UI
	load("hover", "assets/sounds/hover.wav", m_mixerUI);
}

//! Subscribes to EventCardHovered, EventCardPlayed, EventGameOver, EventRoundTied, EventTurnChanged.
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

//! Subscribes to EventButtonHovered.
void AudioSystem::bindEvents(MainMenu& menu) {
	menu.subscribe<EventButtonHovered>([&](const EventButtonHovered& e) {
		onButtonHovered(e);
	});
}

//! Triggered only when \c e.index != -1 (i.e. a button is actually hovered).
void AudioSystem::onButtonHovered(const EventButtonHovered& e) {
	if (e.index != -1) {
		play("hover");
	}
}

//! Triggered only when \c e.index != -1 (i.e. a card is actually hovered).
void AudioSystem::onCardHovered(const EventCardHovered& e) {
	if (e.index != -1)
		play("hover");
}

/*!
	Plays a sound matching the card type and value. Only fires for
	player-played cards (\c e.byPlayer must be \c true).
	Number cards play the same sound; special cards
	(Strike, Flip, Snatch, Double) each have a dedicated sound.
*/
void AudioSystem::onCardPlayed(const EventCardPlayed& e) {
	if (!e.byPlayer) return;
	switch (e.card.type) {
		case CardType::Number:
			play("card_number"); break;
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

//! Plays \c "win" or \c "lose" based on \c e.playerWon.
void AudioSystem::onGameOver(const EventGameOver& e) {
	play(e.playerWon ? "win" : "lose");
}

//! Plays \c "tie".
void AudioSystem::onRoundTied(const EventRoundTied& e) {
	play("tie");
}

//! Plays \c "turn_player" or \c "turn_ai" based on \c e.isPlayerTurn.
void AudioSystem::onTurnChanged(const EventTurnChanged& e) {
	play(e.isPlayerTurn ? "turn_player" : "turn_ai");
}