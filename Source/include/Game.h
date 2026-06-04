#pragma once
#include <SDL3/SDL.h>
#include "GameState.h"
#include "Board.h"
#include "Subscribable.h"
#include "Difficulty.h"

/*!
	\class Game
	\brief Core game state machine and logic controller

	Manages a full game session: round initialization, card dealing, turn flow,
	AI decision-making, and win/tie detection. Inherits \c Subscribable to expose
	an event bus for external systems such as \c AudioSystem and \c DebugSystem.
*/
class Game : public Subscribable {
public:
	/*!
		\brief Constructs a Game and starts the first round
		\param renderer   SDL renderer passed to the Board
		\param difficulty AI difficulty level for this session
	*/
	Game(SDL_Renderer* renderer, Difficulty difficulty);

	//! Processes SDL input events (mouse motion and clicks)
	void handleEvent(const SDL_Event& e);

	//! Advances game logic by one frame; drives AI turn timing
	void update();

	//! Delegates a full frame draw to the Board
	void render();

	//! Returns a reference to the Board for external renderer access
	Board& getBoard() { return m_board; }

private:
	GameState m_state; //!< Full snapshot of the current game state
	Board m_board; //!< Renderer for all in-game visuals

	int m_hoveredCard = -1; //!< Index of the player's currently hovered hand card
	int m_snatchHoveredCard = -1; //!< Index of the opponent's card hovered during Snatch
	int m_selectedCard = -1; //!< Index of the card selected for play
	GamePhase m_snatchCallerTurn = GamePhase::PlayerTurn; //!< Phase active before entering SelectingSnatchTarget; restored after Snatch resolves
	Difficulty m_difficulty; //!< AI difficulty for this session
	float m_aiBestMoveChance; //!< Cached optimal-play probability for the current difficulty

	uint64_t m_aiMoveTime = 0; //!< Timestamp (ms) when the AI move was scheduled
	static constexpr uint64_t AI_DELAY_MS = 1000; //!< Delay in ms before the AI plays its card

	//! \name Round management
	//! \{
	//! Resets and deals a new round, including opening card draw
	void initRound();

	//! Removes all cards from both field stacks
	void clearField();

	//! Builds and returns a freshly shuffled full deck
	std::vector<Card> buildDeck();

	//! Draws one opening card per player and determines who goes first
	void dealOpeningCards();
	//! \}

	//! \name Turn validation
	//! \{
	//! Checks whether the player has any legal card to play; ends the round if not
	void checkPlayerCanPlay();

	//! Checks whether the AI has any legal card to play; ends the round if not
	void checkAICanPlay();

	//! \}

	//! \name Card play
	//! \{
	/*!
		\brief Plays the card at \a handIndex from the player's hand
		\param handIndex Index into the player's hand
	*/
	void playerPlayCard(int handIndex);

	/*!
		Executes the AI's move after \c AI_DELAY_MS has elapsed.
		If the chosen card triggers a Snatch, the AI immediately resolves
		the target selection by picking the highest-value card from the
		player's hand, scaled by \c m_aiBestMoveChance.
		Emits \c EventGameOver if the AI has no legal move.
	*/
	void aiTakeTurn();

	/*!
		Scores each card in the AI's hand based on the current game state
		and returns the index of the chosen card. With probability
		\c m_aiBestMoveChance the highest-scoring move is picked;
		otherwise a weighted random selection is made among the remaining
		valid moves. Returns -1 if no legal move exists.
	*/
	int aiChooseCard();

	/*!
		\brief Sorts \a deck in ascending order with special cards placed at the end
		\param deck Deck to sort in place
	*/
	void sortDeck(std::vector<Card>& deck);

	//! \}

	//! \name Game logic
	//! \{
	/*!
		\brief Calculates the total score for a field
		\param field Field stack to evaluate
		\return Sum of card values after applying all special cards
	*/
	int calcFieldScore(const std::vector<Card>& field) const;

	/*!
		\brief Returns whether \a attacker can play a card that surpasses \a defender's score
		\param attacker Player attempting to surpass
		\param defender Player whose score must be beaten
	*/
	bool canSurpass(const PlayerState& attacker, const PlayerState& defender) const;

	/*!
		\brief Applies the card at \a handIndex from \a attacker's hand to the game state
		\param attacker  Player playing the card
		\param defender  Opposing player
		\param handIndex Index of the card in the attacker's hand
	*/
	void applyCard(PlayerState& attacker, PlayerState& defender, int handIndex);

	/*!
		\brief Resolves a Snatch selection by removing the chosen opponent card
		\param cardIndex Index of the opponent's hand card to remove
	*/
	void handleSnatchSelection(int cardIndex);

	//! Evaluates end-of-round conditions and emits the appropriate events
	void checkRoundEnd();

	//! \}

	//! \name Hit testing
	//! \{
	/*!
		\brief Returns the hand card index under the given mouse position, or -1 if none
		\param mx Mouse X coordinate
		\param my Mouse Y coordinate
	*/
	int cardHitTest(float mx, float my) const;

	/*!
		\brief Returns the opponent hand card index under the given mouse position during Snatch, or -1 if none
		\param mx Mouse X coordinate
		\param my Mouse Y coordinate
	*/
	int snatchHitTest(float mx, float my) const;
	//! \}
};