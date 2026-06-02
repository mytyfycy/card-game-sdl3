#include "Game.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <random>
#include <numeric>
#include <ranges>
#include "Layout.h"
#include <GameEvents.h>

Game::Game(SDL_Renderer* renderer, Difficulty difficulty) 
	: m_board(renderer, "assets/fonts/OpenSans.ttf"),
	m_difficulty(difficulty),
	m_aiBestMoveChance(difficultyToChance(difficulty))
{
	initRound();
}

// Talia:
// 2x karta 1-7 = 14 kart
// + 1x Strike + 1x Flip + 1x Snatch + 1x Double = 18 kart
// Losujemy 10

std::vector<Card> Game::buildDeck() {
	std::vector<Card> pool;

	for (int v = 1; v <= 7; ++v) {
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

void Game::sortDeck(std::vector<Card>& deck) {
	std::ranges::stable_sort(deck, {}, [](const Card& c) {
		return std::tuple{
			c.type != CardType::Number,
			c.type,
			c.value
		};
	});
}

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

// Dobieranie kart na start:
// Kazdy wyklada pierwsza z reki
// Nizszy numer zaczyna
void Game::dealOpeningCards() {

	// Budujemy mala pule by z niej losowac karty
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

	// Jesli ai pierwsze wykonaj ruch
	if (m_state.phase == GamePhase::OpponentTurn)
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
}

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

bool Game::canSurpass(const PlayerState& attacker, int defenderScore) const {
	for (const auto& c : attacker.hand) {
		int val = (c.type == CardType::Number) ? c.value : 1;
		if (attacker.score + val > defenderScore)
			return true;
	}
	return false;
}

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
		// Usuwa ostatnia karte polozona przez przeciwnika
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
		// Zamienia karty graczy
		if (!defender.field.empty() && !attacker.field.empty()) {
			
			// Wyjmij Flip
			attacker.field.pop_back();

			std::swap(attacker.field, defender.field);
			
			attacker.score = calcFieldScore(attacker.field);
			defender.score = calcFieldScore(defender.field);
		}
		break;

	case CardType::Snatch:
		if (!defender.hand.empty()) {
			// Przetasuj reke przeciwnika
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

void Game::checkRoundEnd() {
	bool isPlayerTurn = (m_state.phase == GamePhase::PlayerTurn);
	auto& curr = isPlayerTurn ? m_state.player : m_state.opponent;
	auto& opp = isPlayerTurn ? m_state.opponent : m_state.player;

	// Remis
	if (m_state.player.score == m_state.opponent.score) {
		m_state.phase = GamePhase::RoundEnd;
		clearField();
		dealOpeningCards();
		m_dispatcher.emit(EventRoundTied{});
		return;
	}

	// Ostatnia zagrana karta nie przebija wyniku
	if (curr.score <= opp.score) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = isPlayerTurn ? GameResult::PlayerLose : GameResult::PlayerWin;
		EventGameOver ev;
		ev.playerWon = m_state.result == GameResult::PlayerWin;
		m_dispatcher.emit(ev);
		return;
	}

	bool cantPlayHigher = curr.hand.empty() || !canSurpass(curr, opp.score);

	// Ostatnia karta efekt to przegrana
	bool lastPlayedEffect = !curr.field.empty() &&
		curr.field.back().type != CardType::Number &&
		curr.hand.empty();

	if (cantPlayHigher || lastPlayedEffect) {
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

	if (m_state.phase == GamePhase::OpponentTurn)
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
}

void Game::playerPlayCard(int handIndex) {
	if (m_state.phase != GamePhase::PlayerTurn) return;
	if (handIndex < 0 || handIndex >= static_cast<int>(m_state.player.hand.size())) return;

	applyCard(m_state.player, m_state.opponent, handIndex);

	if (m_state.phase == GamePhase::SelectingSnatchTarget) return;

	checkRoundEnd();
}

// AI wybor karty
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
					if (newScore <= playerScore) continue; // Nie przebija
					// Oszczedzanie zbyt duzych kart
					moveScore = static_cast<float>(newScore - playerScore);
				}
				break;
			}

			case CardType::Strike: {
				// Dobra gdy przeciwnik ma duze karty
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

				// Bonus gdy gracz ma malo kart w rece
				float handRatio = 1.f - (static_cast<float>(m_state.player.hand.size()) / 10.f);
				moveScore *= (1.f + handRatio);

				if (lastCard.type == CardType::Double)
					moveScore *= 2.f;

				break;
			}

			case CardType::Flip: {
				// Dobra gdy gracz ma wiecej punktow
				if (m_state.player.field.empty()) continue;
				if (playerScore <= opponentScore) continue;
				moveScore = static_cast<float>(playerScore - opponentScore);
				break;
			}
			case CardType::Snatch: {
				// Dobra gdy gracz ma duzo kart
				if (m_state.player.hand.empty() &&
					m_state.opponent.hand.size() > m_state.player.hand.size()+1) continue;
				moveScore = static_cast<float>(m_state.player.hand.size()) * 0.8f;
				break;
			}
			case CardType::Double: {
				// Dobra gdy AI juz ma sporo punktow
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

	// Wybieranie najlepszego ruchu (szanse skalowane z poziomem trudnosci)
	std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
	std::uniform_real_distribution<float> roll(0.f, 1.f);

	if (roll(rng) < m_aiBestMoveChance || validMoves.size() == 1)
		return validMoves.front().index;

	std::vector<double> weights;
	size_t remainingCount = validMoves.size() - 1;

	// Przypisujemy wagi ruchom
	for (size_t i = 0; i < remainingCount; ++i) {
		// Im dalszy ruch, tym mniejsza waga
		weights.push_back(std::pow(2.0, static_cast<double>(remainingCount - i)));
	}

	// Losowe z pozostalych (z uwzglednieniem wag)
	std::discrete_distribution<int> pick(weights.begin(), weights.end());
	return validMoves[pick(rng) + 1].index;
}

void Game::aiTakeTurn() {
	int idx = aiChooseCard();

	if (idx == -1) {
		// AI nie moze przebic, koniec
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
				// Najlepsza karta
				chosenIdx = indices.front();
			}
			else {
				// Losowe z pozostalych (z uwzglednieniem wag)
				std::vector<double> weights;
				size_t remainingCount = indices.size() - 1;

				for (size_t i = 0; i < remainingCount; ++i) {
					// Im dalszy ruch, tym mniejsza waga
					weights.push_back(std::pow(2.0, static_cast<double>(remainingCount - i)));
				}

				// Losowe z pozostalych (z uwzglednieniem wag)
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

// Ktory indeks karty kliknal gracz
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

void Game::handleEvent(const SDL_Event& e) {

	// R = restart
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

// AI czeka na swoj czas
void Game::update() {
	if (m_state.phase == GamePhase::OpponentTurn) {
		if (SDL_GetTicks() >= m_aiMoveTime)
			aiTakeTurn();
	}
}

void Game::render() {
	m_board.render(m_state);
}