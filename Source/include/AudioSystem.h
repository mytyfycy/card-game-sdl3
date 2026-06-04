#pragma once
#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
#include <string>
#include "Card.h"
#include "GameEvents.h"
#include "MenuEvents.h"

class Game;
class MainMenu;

/*!
	\class AudioSystem
	\brief Manages sound playback for game and menu contexts

	Loads and plays audio assets through two separate SDL mixer channels:
	one for sound effects and one for UI sounds. Subscribes to game and
	menu events to trigger appropriate sounds automatically.
*/
class AudioSystem {
public:
	//! Full volume level (1.)
	static constexpr float MAX_VOLUME = 1.0f;

	//! Medium volume level (0.66)
	static constexpr float MID_VOLUME = 0.66f;

	//! Low volume level (0.05), used by the UI mixer by default
	static constexpr float LOW_VOLUME = 0.05f;

	/*
		\brief Constructs AudioSystem and initializes SDL mixer channels

		Creates two mixer devices: one for SFX and one for UI sounds.
		Sets the UI mixer gain to \c LOW_VOLUME.
	*/
	AudioSystem();

	//! Destroys all loaded audio assets and releases mixer resources
	~AudioSystem();

	/*!
		\brief Loads game audio assets into the appropriate mixers
		\param game Reference to the active Game instance
	*/
	void init(Game& game);

	/*!
		\brief Loads menu audio assets into the appropriate mixers
		\param menu Reference to the active MainMenu instance
	*/
	void init(MainMenu& menu);

	/*!
		\brief Subscribes to game events to trigger sound playback
		\param game Reference to the active Game instance
	*/
	void bindEvents(Game& game);

	/*!
		\brief Subscribes to menu events to trigger sound playback
		\param menu Reference to the active MainMenu instance
	*/
	void bindEvents(MainMenu& menu);

	/*!
		\brief Plays a previously loaded sound by its identifier
		\param id Key used when the sound was loaded via \c load()
	*/
	void play(const std::string& id);

private:
	MIX_Mixer* m_mixerSFX = nullptr; //!< Mixer channel for sound effects
	MIX_Mixer* m_mixerUI = nullptr; //!< Mixer channel for UI sounds

	/*!
		\brief Associates a loaded audio asset with its mixer
	*/
	struct SoundEntry {
		MIX_Audio* audio; //!< Loaded audio asset
		MIX_Mixer* mixer; //!< Mixer to play the asset through
	};

	//! Map of sound identifiers to their loaded assets and mixers
	std::unordered_map<std::string, SoundEntry> m_sounds;

	/*!
		\brief Loads an audio file and registers it under the given identifier
		\param id Key used to reference the sound later
		\param path Path to the audio file on disk
		\param mixer Mixer to assign the sound to
	*/
	void load(const std::string& id, const std::string& path, MIX_Mixer* mixer);

	//! \name Game event handlers
	//! \{
	void onCardHovered(const EventCardHovered& e); //!< Plays hover sound when a card is highlighted
	void onCardPlayed(const EventCardPlayed& e); //!< Plays a card-type-specific sound when the player plays a card
	void onGameOver(const EventGameOver& e); //!< Plays win or lose sound depending on outcome
	void onRoundTied(const EventRoundTied& e); //!< Plays tie sound on a drawn round
	void onTurnChanged(const EventTurnChanged& e); //!< Plays turn cue sound for player or AI turn
	//! \}

	//! \name Menu event handlers
	//! \{
	void onButtonHovered(const EventButtonHovered& e); //!< Plays hover sound when a menu button is highlighted
	//! \}
};