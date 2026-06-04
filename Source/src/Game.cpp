/*!
	\file Game.cpp
	\brief Implementation of Game

	Covers the full game loop: round setup, card dealing, player and AI
	turn logic, special card effects, Snatch resolution, and end-of-round
	detection. All state changes that external systems need to react to
	are broadcast via the inherited \c Subscribable dispatcher.
*/

#include "Game.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <random>
#include <numeric>
#include <ranges>
#include "Layout.h"
#include <GameEvents.h>

/*!
	Caches \c difficultyToChance() result in \c m_aiBestMoveChance
	and calls \c initRound() to deal the first round immediately.
*/
Game::Game(SDL_Renderer* renderer, Difficulty difficulty) 
	: m_board(renderer, "assets/fonts/OpenSans.ttf"),
	m_difficulty(difficulty),
	m_aiBestMoveChance(difficultyToChance(difficulty))
{
	initRound();
}

/*!
	Full deck pool: two copies of Number cards 1–7 (14 cards) plus one
	each of Strike, Flip, Snatch, and Double (18 total). Shuffled with
	a time-seeded Mersenne Twister and trimmed to 10 cards.
*/
std::vector<Card> Game::buildDeck() {
	std::vector<Card> pool;

	int HIGHEST_NUMBER_CARD = 7;

	for (int v = 1; v <= HIGHEST_NUMBER_CARD; ++v) {
		std::string path = "assets/textures/card_" + std::to_string(v) + ".png";
		for (int i = 0; i < 2; ++i)
			pool.push_back({ CardType::Number, v, path });
	}

	pool.push_back({ CardType::Strike, 1, "assets/textures/card_strike.png"});
	pool.push_back({ CardType::Flip, 1, "assets/textures/card_flip.png" });
	pool.push_back({ CardType::Snatch, 1, "assets/textures/card_snatch.png" });
	pool.push_back({ CardType::Double, 1, "assets/textures/card_double.png" });

	std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
	std::shuffle(pool.begin(), pool.end(), rng);

	pool.resize(10);
	return pool;
}

/*!
	Sorts using a stable sort on a tuple key: Number cards first
	(ordered by value), then special cards (ordered by type).
*/
void Game::sortDeck(std::vector<Card>& deck) {
	std::ranges::stable_sort(deck, {}, [](const Card& c) {
		return std::tuple{
			c.type != CardType::Number,
			c.type,
			c.value
		};
	});
}

/*!
	Deals a fresh deck to both players, sorts the player's hand,
	clears the field, resets hover/selection state, then calls
	\c dealOpeningCards() to determine who goes first.
*/
void Game::initRound() {
	m_state.player.hand = buildDeck();
	m_state.opponent.hand = buildDeck();
	sortDeck(m_state.player.hand);

	clearField();

	m_state.phase = GamePhase::Setup;
	m_hoveredCard = -1;
	m_selectedCard = -1;

	dealOpeningCards();
}

/*!
	Clears both field stacks and resets scores, last-snatched,
	last-struck, and struckThisTurn flags for both players.
*/
void Game::clearField() {
	m_state.player.field.clear();
	m_state.player.score = 0;

	m_state.opponent.field.clear();
	m_state.opponent.score = 0;

	m_state.lastSnatchedCard.reset();

	m_state.player.lastStruck.reset();
	m_state.player.struckThisTurn = false;

	m_state.opponent.lastStruck.reset();
	m_state.opponent.struckThisTurn = false;
}

/*!
	Draws two distinct random values from 1–7, places one on each
	player's field, and sets the phase to \c PlayerTurn if the player
	drew lower, otherwise \c OpponentTurn. Immediately calls the
	appropriate can-play check and schedules the AI move if needed.
*/
void Game::dealOpeningCards() {

	std::vector<int> pool = { 1,2,3,4,5,6,7 };
	std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
	std::shuffle(pool.begin(), pool.end(), rng);

	int pVal = pool[0];
	int oVal = pool[1];

	Card playerOpen = { CardType::Number, pVal, "assets/textures/card_" + std::to_string(pVal) + ".png"};
	Card opponentOpen = { CardType::Number, oVal, "assets/textures/card_" + std::to_string(oVal) + ".png" };

	m_state.player.field.push_back(playerOpen);
	m_state.opponent.field.push_back(opponentOpen);

	m_state.player.score = pVal;
	m_state.opponent.score = oVal;

	m_state.phase = (pVal < oVal) ? GamePhase::PlayerTurn : GamePhase::OpponentTurn;

	if (m_state.phase == GamePhase::OpponentTurn) {

		checkAICanPlay();

		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
	}
	else {
		checkPlayerCanPlay();
	}
}

/*!
	Iterates the field left to right. Number card values are summed
	normally; a Double card doubles the running total at the point
	it appears. Non-numeric special cards contribute 0.
*/
int Game::calcFieldScore(const std::vector<Card>& field) const {
	int score = 0;
	for (size_t i = 0; i < field.size(); ++i) {
		int val = (field[i].type == CardType::Number) ? field[i].value : 0;
		if (field[i].type == CardType::Double)
			score = (score + val) * 2;
		else
			score += val;
	}
	return score;
}

/*!
	Checks each card in \a attacker's hand. A Number card surpasses
	if \c attacker.score + value >= \c defender.score. Special cards
	are evaluated against their specific effect outcome (Double doubles
	attacker score, Flip swaps scores, Strike removes defender's last card).
*/
bool Game::canSurpass(const PlayerState& attacker, const PlayerState& defender) const {
	for (const auto& c : attacker.hand) {
		int val = (c.type == CardType::Number) ? c.value : 0;
		if (c.type == CardType::Double && attacker.score * 2 > defender.score) return true;
		if (c.type == CardType::Flip && defender.score > attacker.score) return true;
		if (c.type == CardType::Strike && !defender.field.empty() && attacker.score > defender.score - defender.field.back().value) return true;
		if (attacker.score + val >= defender.score) return true;
	}
	return false;
}

/*!
	Removes the card at \a handIndex from \a attacker's hand, pushes it
	onto their field, and applies the card's effect:
	- \b Number: recalculates score; if value is 1 and a Strike was received
	  this turn, restores the struck card instead and emits \c lastRestoredCard
	- \b Strike: removes the last card from \a defender's field, emits
	  \c EventCardRemoved, and discards the Strike from the attacker's field
	- \b Flip: removes the Flip card then swaps both field stacks and recalculates scores
	- \b Snatch: shuffles \a defender's hand (player turn only), transitions to
	  \c SelectingSnatchTarget, and emits \c EventSnatchTargetRequired; returns
	  early without emitting \c EventCardPlayed
	- \b Double: recalculates score (field already contains the Double card)

	Emits \c EventCardPlayed after all non-Snatch effects are applied.
*/
void Game::applyCard(PlayerState& attacker,
	PlayerState& defender,
	int handIndex)
{
	Card played = attacker.hand[handIndex];
	bool isPlayer = (&attacker == &m_state.player);

	attacker.hand.erase(attacker.hand.begin() + handIndex);
	attacker.field.push_back(played);

	m_state.lastPlayedCard = played;
	m_state.lastPlayedByPlayer = isPlayer;

	switch (played.type) {
	case CardType::Number:
		if (played.value == 1 && attacker.lastStruck.has_value() && attacker.struckThisTurn) {
			Card restored = attacker.lastStruck.value();

			attacker.field.pop_back();
			attacker.field.push_back(restored);

			attacker.lastStruck.reset();
			attacker.struckThisTurn = false;
			m_state.lastRestoredCard = restored;
		}
		attacker.score = calcFieldScore(attacker.field);
		break;

	case CardType::Strike:
		if (!defender.field.empty()) {
			Card removed = defender.field.back();
			defender.field.pop_back();
			defender.score = calcFieldScore(defender.field);
			defender.lastStruck = removed;
			defender.struckThisTurn = true;
			EventCardRemoved ev;
			ev.card = removed;
			ev.fromPlayer = (&defender == &m_state.player);
			m_dispatcher.emit(ev);
			attacker.field.pop_back();
		}
		break;

	case CardType::Flip:
		if (!defender.field.empty() && !attacker.field.empty()) {
			attacker.field.pop_back();

			std::swap(attacker.field, defender.field);
			
			attacker.score = calcFieldScore(attacker.field);
			defender.score = calcFieldScore(defender.field);
		}
		break;

	case CardType::Snatch:
		if (!defender.hand.empty()) {
			if (&attacker == &m_state.player) {
				std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
				std::shuffle(defender.hand.begin(), defender.hand.end(), rng);
			}

			m_snatchCallerTurn = m_state.phase;
			m_state.phase = GamePhase::SelectingSnatchTarget;
			m_state.snatchPending = true;
			EventSnatchTargetRequired ev;
			ev.opponentHandSize = defender.hand.size();
			m_dispatcher.emit(ev);
			return;
		}
		break;

	case CardType::Double:
		attacker.score = calcFieldScore(attacker.field);
		break;

	default:
		attacker.score = calcFieldScore(attacker.field);
		break;
	}

	EventCardPlayed ev;
	ev.byPlayer = isPlayer;
	ev.card = played;
	m_dispatcher.emit(ev);
}

/*!
	Validates \a cardIndex against the opponent's hand, removes the card,
	clears \c snatchPending, stores the removed card in \c lastSnatchedCard,
	and emits \c EventSnatchResolved. Restores the phase to \c m_snatchCallerTurn
	and schedules the AI move if it was the opponent's turn.
*/
void Game::handleSnatchSelection(int cardIndex) {
	if (m_state.phase != GamePhase::SelectingSnatchTarget) return;
	if (cardIndex < 0 || cardIndex >= static_cast<int>(m_state.opponent.hand.size())) return;

	Card removedCard = m_state.opponent.hand[cardIndex];
	m_state.opponent.hand.erase(m_state.opponent.hand.begin() + cardIndex);

	m_state.snatchPending = false;
	m_state.lastSnatchedCard = removedCard;

	EventSnatchResolved ev;
	ev.removedCard = removedCard;
	m_dispatcher.emit(ev);

	m_state.phase = m_snatchCallerTurn;

	m_hoveredCard = -1;
	EventCardHovered ev_h;
	ev_h.index = -1;
	m_dispatcher.emit(ev_h);

	if (m_state.phase == GamePhase::OpponentTurn)
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
}

/*!
	Checks in order: tie (equal scores → clear field, new round, emit
	\c EventRoundTied), current player failed to surpass (emit \c EventGameOver),
	special card played as final move / empty hand foul (emit \c EventGameOver).
	If the round continues, advances the phase, resets per-turn flags,
	emits \c EventTurnChanged, and schedules the next can-play check.
*/
void Game::checkRoundEnd() {
	bool isPlayerTurn = (m_state.phase == GamePhase::PlayerTurn);
	auto& curr = isPlayerTurn ? m_state.player : m_state.opponent;
	auto& opp = isPlayerTurn ? m_state.opponent : m_state.player;

	if (m_state.player.score == m_state.opponent.score) {
		m_state.phase = GamePhase::RoundEnd;
		clearField();
		dealOpeningCards();
		m_dispatcher.emit(EventRoundTied{});
		return;
	}

	if (curr.score <= opp.score) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = isPlayerTurn ? GameResult::PlayerLose : GameResult::PlayerWin;
		EventGameOver ev;
		ev.playerWon = m_state.result == GameResult::PlayerWin;
		m_dispatcher.emit(ev);
		return;
	}

	if (isPlayerTurn) {
		m_state.player.struckThisTurn = false;
		m_state.lastSnatchedCard.reset();
		m_state.lastRestoredCard.reset();
	}
	else {
		m_state.opponent.struckThisTurn = false;
	}

	m_state.phase = isPlayerTurn ? GamePhase::OpponentTurn : GamePhase::PlayerTurn;

	EventTurnChanged ev;
	ev.isPlayerTurn = m_state.phase == GamePhase::PlayerTurn;
	m_dispatcher.emit(ev);

	if (m_state.phase == GamePhase::OpponentTurn) {
		checkAICanPlay();
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
	}

	if (m_state.phase == GamePhase::PlayerTurn)
		checkPlayerCanPlay();
}

/*!
	Triggers \c EventGameOver with \c playerWon = false if the player
	has no legal move or ends their turn on a special card with an empty hand.
*/
void Game::checkPlayerCanPlay() {
	bool cantPlayHigher = m_state.player.hand.empty() || !canSurpass(m_state.player, m_state.opponent);

	bool lastPlayedEffect = !m_state.player.field.empty() &&
		m_state.player.field.back().type != CardType::Number &&
		m_state.player.hand.empty();

	if (cantPlayHigher || lastPlayedEffect) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = GameResult::PlayerLose;
		EventGameOver ev;
		ev.playerWon = false;
		m_dispatcher.emit(ev);
		return;
	}
}

/*!
	Triggers \c EventGameOver with \c playerWon = true if the AI
	has no legal move or ends its turn on a special card with an empty hand.
*/
void Game::checkAICanPlay() {
	bool cantPlayHigher = m_state.opponent.hand.empty() || !canSurpass(m_state.opponent, m_state.player);

	bool lastPlayedEffect = !m_state.opponent.field.empty() &&
		m_state.opponent.field.back().type != CardType::Number &&
		m_state.opponent.hand.empty();

	if (cantPlayHigher || lastPlayedEffect) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = GameResult::PlayerWin;
		EventGameOver ev;
		ev.playerWon = true;
		m_dispatcher.emit(ev);
		return;
	}
}

/*!
	Guards against wrong phase and out-of-range index.
	Calls \c applyCard() then \c checkRoundEnd(), unless a Snatch
	was triggered (in which case \c applyCard already returned early).
*/
void Game::playerPlayCard(int handIndex) {
	if (m_state.phase != GamePhase::PlayerTurn) return;
	if (handIndex < 0 || handIndex >= static_cast<int>(m_state.player.hand.size())) return;

	applyCard(m_state.player, m_state.opponent, handIndex);

	if (m_state.phase == GamePhase::SelectingSnatchTarget) return;

	checkRoundEnd();
}

/*!
	Scores each card in the AI's hand:
	- \b Number: margin over player score; skipped if it doesn't surpass;
	  value-1 restore path scored separately if a Strike was received this turn
	- \b Strike: estimated value of the card it would remove, boosted by
	  hand-size ratio and doubled if the target is a Double card; skipped
	  if the player's score is below 8
	- \b Flip: score difference (skipped if AI is already ahead)
	- \b Snatch: proportional to player hand size (skipped if AI hand is larger)
	- \b Double: margin after doubling (skipped if result doesn't surpass)

	Moves are sorted descending by score. With probability \c m_aiBestMoveChance
	the top move is returned; otherwise a weighted random pick is made among
	the remaining moves using exponentially decreasing weights.
	Returns -1 if no valid moves exist.
*/
int Game::aiChooseCard() {
	
	struct ScoredMove {
		int index;
		float score;
	};

	std::vector<ScoredMove> validMoves;
	int playerScore = m_state.player.score;
	int opponentScore = m_state.opponent.score;

	for (int i = 0; i < static_cast<int>(m_state.opponent.hand.size()); ++i) {
		const Card& c = m_state.opponent.hand[i];

		float moveScore = 0.f;

		switch (c.type) {
			case CardType::Number: {
				if (c.value == 1 && m_state.opponent.lastStruck.has_value() && m_state.opponent.struckThisTurn) {
					Card struckCard = m_state.opponent.lastStruck.value();

					float struckVal = 0.f;

					switch (struckCard.type) {
						case CardType::Number: struckVal = static_cast<float>(struckCard.value); break;
						case CardType::Double: struckVal = static_cast<float>(m_state.opponent.score) * 2.f; break;
						case CardType::Snatch:
						case CardType::Flip:
						case CardType::Strike: struckVal = 0.f; break;
						default: struckVal = 1; break;
					}

					if (opponentScore + struckVal <= playerScore) continue;

					moveScore = struckVal * 1.5f;

					if (struckCard.type == CardType::Double)
						moveScore *= 2.f;
				}
				else {
					int newScore = opponentScore + c.value;
					if (newScore <= playerScore) continue;
					moveScore = static_cast<float>(newScore - playerScore);
				}
				break;
			}

			case CardType::Strike: {
				if (m_state.player.field.empty()) continue;

				Card lastCard = m_state.player.field.back();

				float removedVal = 0.f;

				switch (lastCard.type) {
					case CardType::Number: removedVal = static_cast<float>(lastCard.value); break;
					case CardType::Double: removedVal = static_cast<float>(playerScore) * 0.5f; break;
					case CardType::Snatch:
					case CardType::Flip:
					case CardType::Strike: removedVal = 0.f; break;
					default: removedVal = 1; break;
				}

				if (playerScore < 8.f) continue;

				int newPlayerScore = playerScore - static_cast<int>(removedVal);
				if (newPlayerScore < 0) newPlayerScore = 0;

				if (opponentScore <= newPlayerScore) continue;

				moveScore = removedVal * 1.5f;

				float handRatio = 1.f - (static_cast<float>(m_state.player.hand.size()) / 10.f);
				moveScore *= (1.f + handRatio);

				if (lastCard.type == CardType::Double)
					moveScore *= 2.f;

				break;
			}

			case CardType::Flip: {
				if (m_state.player.field.empty()) continue;
				if (playerScore <= opponentScore) continue;
				moveScore = static_cast<float>(playerScore - opponentScore);
				break;
			}
			case CardType::Snatch: {
				if (m_state.player.hand.empty() ||
					m_state.opponent.hand.size() > m_state.player.hand.size()+1) continue;
				moveScore = static_cast<float>(m_state.player.hand.size()) * 0.8f;
				break;
			}
			case CardType::Double: {
				int newScore = opponentScore * 2;
				if (newScore <= playerScore) continue;
				moveScore = static_cast<float>(newScore - playerScore);
				break;
			}
			default: continue;
		}

		validMoves.push_back({ i, moveScore });
	}

	if (validMoves.empty()) return -1;

	std::sort(validMoves.begin(), validMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
		return a.score > b.score;
		});

	std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
	std::uniform_real_distribution<float> roll(0.f, 1.f);

	if (roll(rng) < m_aiBestMoveChance || validMoves.size() == 1)
		return validMoves.front().index;

	std::vector<double> weights;
	size_t remainingCount = validMoves.size() - 1;

	for (size_t i = 0; i < remainingCount; ++i) {
		weights.push_back(std::pow(2.0, static_cast<double>(remainingCount - i)));
	}

	std::discrete_distribution<int> pick(weights.begin(), weights.end());
	return validMoves[pick(rng) + 1].index;
}

/*!
	Calls \c aiChooseCard(); if no move is found, sets \c GameOver (player wins)
	and returns. Otherwise calls \c applyCard(). If a Snatch was triggered,
	the AI immediately resolves the target: cards are ranked by estimated value
	(Double > Strike > Flip > Snatch > Number by value) and the best or a
	weighted-random card is removed from the player's hand using the same
	\c m_aiBestMoveChance probability. Emits \c EventSnatchResolved and
	reschedules the AI move after \c AI_DELAY_MS.
	If no Snatch, calls \c checkRoundEnd().
*/
void Game::aiTakeTurn() {
	int idx = aiChooseCard();

	if (idx == -1) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = GameResult::PlayerWin;
		EventGameOver ev;
		ev.playerWon = true;
		m_dispatcher.emit(ev);
		return;
	}

	applyCard(m_state.opponent, m_state.player, idx);

	if (m_state.phase == GamePhase::SelectingSnatchTarget) {
		if (!m_state.player.hand.empty()) {
			std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));

			std::vector<int> indices(m_state.player.hand.size());
			std::iota(indices.begin(), indices.end(), 0);
			std::sort(indices.begin(), indices.end(), [&](int a, int b) {
				const Card& ca = m_state.player.hand[a];
				const Card& cb = m_state.player.hand[b];

				int va = (ca.type == CardType::Number) ? ca.value : 1;
				int vb = (cb.type == CardType::Number) ? cb.value : 1;

				if (ca.type == CardType::Double) va = 10;
				if (cb.type == CardType::Double) vb = 10;

				if (ca.type == CardType::Strike) va = 8;
				if (cb.type == CardType::Strike) vb = 8;

				if (ca.type == CardType::Flip) va = 7;
				if (cb.type == CardType::Flip) vb = 7;

				if (ca.type == CardType::Snatch) va = 5;
				if (cb.type == CardType::Snatch) vb = 5;

				return va > vb;
			});

			std::uniform_real_distribution<float> roll(0.f, 1.f);

			int chosenIdx = -1;

			if (roll(rng) < m_aiBestMoveChance || indices.size() == 1) {
				chosenIdx = indices.front();
			}
			else {
				std::vector<double> weights;
				size_t remainingCount = indices.size() - 1;

				for (size_t i = 0; i < remainingCount; ++i) {
					weights.push_back(std::pow(2.0, static_cast<double>(remainingCount - i)));
				}

				std::discrete_distribution<int> pick(weights.begin(), weights.end());
				chosenIdx = indices[pick(rng) + 1];
			}

			Card removed = m_state.player.hand[chosenIdx];
			m_state.player.hand.erase(m_state.player.hand.begin() + chosenIdx);

			m_state.lastSnatchedCard = removed;
			m_state.snatchPending = false;

			EventSnatchResolved ev;
			ev.removedCard = removed;
			m_dispatcher.emit(ev);
		}

		m_state.phase = m_snatchCallerTurn;
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
		return;
	}
	
	checkRoundEnd();
}

/*!
	Tests each card in the player's hand against its screen rect computed
	from \c Layout::PLY_HAND_X/Y() and card dimensions.
*/
int Game::cardHitTest(float mx, float my) const {
	const auto& hand = m_state.player.hand;

	for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
		float cx = Layout::PLY_HAND_X() + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		float cy = Layout::PLY_HAND_Y();
		if (mx >= cx && mx <= cx + Layout::HAND_CARD_W() &&
			my >= cy && my <= cy + Layout::HAND_CARD_H())
			return i;
	}
	return -1;
}

/*!
	Tests each card in the opponent's hand against its screen rect computed
	from \c Layout::OPP_HAND_X/Y() and card dimensions.
*/
int Game::snatchHitTest(float mx, float my) const {
	const auto& hand = m_state.opponent.hand;

	for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
		float cx = Layout::OPP_HAND_X() + i * (Layout::HAND_CARD_W() + Layout::HAND_GAP());
		float cy = Layout::OPP_HAND_Y();
		if (mx >= cx && mx <= cx + Layout::HAND_CARD_W() && my >= cy && my <= cy + Layout::HAND_CARD_H())
			return i;
	}
	return -1;
}

/*!
	R key triggers \c initRound() regardless of phase.
	During \c SelectingSnatchTarget: routes clicks to \c handleSnatchSelection()
	and motion to \c snatchHitTest(), emitting \c EventSnatchCardHovered on change.
	During \c PlayerTurn: routes clicks to \c playerPlayCard() and motion to
	\c cardHitTest(), emitting \c EventCardHovered on change.
	All other phases are ignored.
*/
void Game::handleEvent(const SDL_Event& e) {

	if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_R) {
		initRound();
		return;
	}

	if (m_state.phase == GamePhase::SelectingSnatchTarget) {
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
			int idx = snatchHitTest(e.button.x, e.button.y);
			if (idx != -1) handleSnatchSelection(idx);
		}
		if (e.type == SDL_EVENT_MOUSE_MOTION) {
			int idx = snatchHitTest(e.motion.x, e.motion.y);
			if (idx != m_snatchHoveredCard) {
				m_snatchHoveredCard = idx;
				EventSnatchCardHovered ev;
				ev.index = idx;
				m_dispatcher.emit(ev);
			}
		}
		return;
	}

	if (m_state.phase != GamePhase::PlayerTurn) return;

	if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
		int idx = cardHitTest(e.button.x, e.button.y);
		if (idx != -1)
			playerPlayCard(idx);
	}

	if (e.type == SDL_EVENT_MOUSE_MOTION) {
		int idx = cardHitTest(e.motion.x, e.motion.y);
		if (idx != m_hoveredCard) {
			m_hoveredCard = idx;
			EventCardHovered ev;
			ev.index = idx;
			m_dispatcher.emit(ev);
		}
	}
}

//! Fires \c aiTakeTurn() once \c SDL_GetTicks() >= \c m_aiMoveTime.
void Game::update() {
	if (m_state.phase == GamePhase::OpponentTurn) {
		if (SDL_GetTicks() >= m_aiMoveTime)
			aiTakeTurn();
	}
}

//! Delegates to \c m_board.render().
void Game::render() {
	m_board.render(m_state);
}