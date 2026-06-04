#pragma once
#include <vector>
#include "Card.h"
#include <optional>

/*!
	\enum GamePhase
	\brief Represents the current phase of a game session

	\var GamePhase::Setup                  Initial state before the first turn is determined
	\var GamePhase::PlayerTurn             Player is choosing a card to play
	\var GamePhase::OpponentTurn           AI is waiting to play its card
	\var GamePhase::SelectingSnatchTarget  Player or AI must select an opponent card to remove
	\var GamePhase::RoundEnd               Tie detected; field is being cleared for a new round
	\var GamePhase::GameOver               The game has ended with a result
*/
enum class GamePhase {
	Setup,
	PlayerTurn,
	OpponentTurn,
	SelectingSnatchTarget,
	RoundEnd,
	GameOver
};

/*!
	\enum GameResult
	\brief Outcome of a finished game

	\var GameResult::None       Game is still in progress
	\var GameResult::PlayerWin  Player won
	\var GameResult::PlayerLose Player lost
*/
enum class GameResult { None, PlayerWin, PlayerLose };

/*!
	\struct PlayerState
	\brief Holds all mutable state belonging to one player
*/
struct PlayerState {
	std::vector<Card> hand; //!< Cards currently in the player's hand
	std::vector<Card> field; //!< Cards played onto the field this round
	int score = 0; //!< Current field score
	std::optional<Card> lastStruck; //!< Last card removed from this player's field by a Strike
	bool struckThisTurn = false; //!< \c true if a Strike was received during the current turn
};

/*!
	\struct GameState
	\brief Complete snapshot of the current game session

	Passed to \c Board::render() each frame and mutated by \c Game
	as the game progresses.
*/
struct GameState {
	PlayerState player; //!< Human player state
	PlayerState opponent; //!< AI opponent state
	GamePhase phase = GamePhase::Setup; //!< Current game phase
	GameResult result = GameResult::None; //!< Final result; valid only in \c GameOver phase
	int round = 1; //!< Current round number

	bool snatchPending = false; //!< \c true while a Snatch target selection is in progress
	std::optional<Card> lastSnatchedCard; //!< Card most recently removed by a Snatch

	bool lastPlayedByPlayer = false; //!< \c true if the last card was played by the player
	std::optional<Card> lastPlayedCard; //!< Most recently played card

	std::optional<Card> lastRestoredCard; //!< Card most recently restored by a value-1 Number card after a Strike
};