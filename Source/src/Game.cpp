#include "Game.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <random>
#include <numeric>
#include <ranges>

Game::Game(SDL_Renderer* renderer) : m_board(renderer, "assets/fonts/OpenSans.ttf")
{
	initRound();
}

// Talia:
// 2x karta 1-7 = 14 kart
// + 2x Bolt + 2x Mirror = 18 kart
// Losujemy 10

std::vector<Card> Game::buildDeck() {
	std::vector<Card> pool;

	for (int v = 1; v <= 7; ++v) {
		for (int i = 0; i < 2; ++i)
			pool.push_back({ CardType::Number, v });
	}

	pool.push_back({CardType::Bolt, 1});
	pool.push_back({ CardType::Bolt, 1 });
	pool.push_back({ CardType::Mirror, 1 });
	pool.push_back({ CardType::Mirror, 1 });

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

	Card playerOpen = { CardType::Number, pVal };
	Card opponentOpen = { CardType::Number, oVal };

	m_state.player.field.push_back(playerOpen);
	m_state.opponent.field.push_back(opponentOpen);

	m_state.player.score = pVal;
	m_state.opponent.score = oVal;

	// Jak remis wylosuj od nowa
	if (pVal == oVal) {
		clearField();
		dealOpeningCards();
		return;
	}

	m_state.phase = (pVal < oVal) ? GamePhase::PlayerTurn : GamePhase::OpponentTurn;

	// Jesli ai pierwsze wykonaj ruch
	if (m_state.phase == GamePhase::OpponentTurn)
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
}

int Game::calcFieldScore(const std::vector<Card>& field) const {
	int sum = 0;
	for (const auto& c : field)
		sum += (c.type == CardType::Number) ? c.value : 1;
	return sum;
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
	attacker.hand.erase(attacker.hand.begin() + handIndex);
	attacker.field.push_back(played);

	switch (played.type) {
	case CardType::Number:
		attacker.score += played.value;
		break;

	case CardType::Bolt:
		// Usuwa ostatnia karte polozona przez przeciwnika
		if (!defender.field.empty()) {
			Card removed = defender.field.back();
			int val = (removed.type == CardType::Number) ? removed.value : 1;
			defender.field.pop_back();
			defender.score -= val;
			if (defender.score < 0) defender.score = 0;
		}

		// Bolt liczy sie jako 1
		attacker.score += 1;
		break;

	case CardType::Mirror:
		// Zamienia karty graczy
		if (!defender.field.empty() && !attacker.field.empty()) {
			
			// Wyjmij mirror
			attacker.field.pop_back();

			std::swap(attacker.field, defender.field);
			std::swap(attacker.score, defender.score);

			// Wloz mirror po swapach
			attacker.field.push_back(played);
		}
		// Mirror liczy sie jako 1
		attacker.score += 1;
		break;

	default:
		// TODO:
		// Blast, Force
		attacker.score += 1;
		break;
	}
}

void Game::checkRoundEnd() {
	bool isPlayerTurn = (m_state.phase == GamePhase::PlayerTurn);
	auto& curr = isPlayerTurn ? m_state.player : m_state.opponent;
	auto& opp = isPlayerTurn ? m_state.opponent : m_state.player;

	// Remis
	if (m_state.player.score == m_state.opponent.score) {
		m_state.phase = GamePhase::RoundEnd;
		SDL_Log("Tie - new round");
		clearField();
		dealOpeningCards();
		return;
	}

	// Ostatnia zagrana karta nie przebija wyniku
	if (curr.score <= opp.score) {
		m_state.phase = GamePhase::GameOver;
		m_state.result = isPlayerTurn ? GameResult::PlayerLose : GameResult::PlayerWin;
		SDL_Log("Game over - did not play a higher card");
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
		SDL_Log("Game end");
		return;
	}

	m_state.phase = isPlayerTurn ? GamePhase::OpponentTurn : GamePhase::PlayerTurn;

	if (m_state.phase == GamePhase::OpponentTurn)
		m_aiMoveTime = SDL_GetTicks() + AI_DELAY_MS;
}

void Game::playerPlayCard(int handIndex) {
	if (m_state.phase != GamePhase::PlayerTurn) return;
	if (handIndex < 0 || handIndex >= static_cast<int>(m_state.player.hand.size())) return;

	applyCard(m_state.player, m_state.opponent, handIndex);
	checkRoundEnd();
}

// AI wybor karty
// Zagraj najmniejsza karte ktora przebija
int Game::aiChooseCard() {
	int bestIdx = -1;
	int bestVal = INT_MAX;

	for (int i = 0; i < static_cast<int>(m_state.opponent.hand.size()); ++i) {
		const Card& c = m_state.opponent.hand[i];
		int val = (c.type == CardType::Number) ? c.value : 1;
		int newScore = m_state.opponent.score + val;

		if (newScore > m_state.player.score && val < bestVal) {
			bestVal = val;
			bestIdx = i;
		}
	}
	return bestIdx; //-1 = AI nie moze zagrac
}

void Game::aiTakeTurn() {
	int idx = aiChooseCard();

	if (idx == -1) {
		// AI nie moze przebic, koniec
		m_state.phase = GamePhase::GameOver;
		m_state.result = GameResult::PlayerWin;
		SDL_Log("AI cannot play higher card - player wins");
		return;
	}

	applyCard(m_state.opponent, m_state.player, idx);
	checkRoundEnd();
}

// Ktory indeks karty kliknal gracz
int Game::cardHitTest(float mx, float my) const {
	using namespace Layout;
	const auto& hand = m_state.player.hand;

	for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
		float cx = PLY_HAND_X + i * (HAND_CARD_W + HAND_GAP);
		float cy = PLY_HAND_Y;
		if (mx >= cx && mx <= cx + HAND_CARD_W &&
			my >= cy && my <= cy + HAND_CARD_H)
			return i;
	}
	return -1;
}

void Game::handleEvent(const SDL_Event& e) {

	// R = restart
	if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_R)
		initRound();

	if (m_state.phase != GamePhase::PlayerTurn) return;

	if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
		int idx = cardHitTest(e.button.x, e.button.y);
		if (idx != -1)
			playerPlayCard(idx);
	}

	if (e.type == SDL_EVENT_MOUSE_MOTION) {
		m_hoveredCard = cardHitTest(e.motion.x, e.motion.y);
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