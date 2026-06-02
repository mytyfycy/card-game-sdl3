#pragma once
#include <memory>
#include <stack>
#include "GameStateBase.h"

class StateManager {
public:
	void push(std::unique_ptr<GameStateBase> state);

	void pop();

	void replace(std::unique_ptr<GameStateBase> state);

	void handleEvent(const SDL_Event& e);
	void update();
	void render();

	bool isEmpty() const { return m_states.empty(); }

	GameStateBase* getCurrentState() const {
		return m_states.empty() ? nullptr : m_states.top().get();
	}

	template <typename T>
	bool isCurrentState() const {
		if (m_states.empty()) return false;
		return dynamic_cast<const T*>(m_states.top().get()) != nullptr;
	}

private:
	std::stack<std::unique_ptr<GameStateBase>> m_states;

	// Operacje wykonywane po update/render zeby nie modyfikowac w trakcie iteracji
	enum class PendingOp { None, Push, Pop, Replace};
	PendingOp m_pendingOp = PendingOp::None;
	std::unique_ptr<GameStateBase> m_pendingState = nullptr;

	void applyPending();
};