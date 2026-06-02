#include "StateManager.h"

void StateManager::push(std::unique_ptr<GameStateBase> state) {
	m_pendingOp = PendingOp::Push;
	m_pendingState = std::move(state);
}

void StateManager::pop() {
	m_pendingOp = PendingOp::Pop;
}

void StateManager::replace(std::unique_ptr<GameStateBase> state) {
	m_pendingOp = PendingOp::Replace;
	m_pendingState = std::move(state);
}

void StateManager::applyPending() {
	switch (m_pendingOp) {
	case PendingOp::Push:
		if (!m_states.empty())
			m_states.top()->onExit();
		m_pendingState->m_manager = this;
		m_pendingState->onEnter();
		m_states.push(std::move(m_pendingState));
		break;

	case PendingOp::Pop:
		if (!m_states.empty()) {
			m_states.top()->onExit();
			m_states.pop();
		}
		if (!m_states.empty())
			m_states.top()->onEnter();
		break;

	case PendingOp::Replace:
		if (!m_states.empty()) {
			m_states.top()->onExit();
			m_states.pop();
		}
		m_pendingState->m_manager = this;
		m_pendingState->onEnter();
		m_states.push(std::move(m_pendingState));
		break;

	case PendingOp::None:
		break;
	}

	m_pendingOp = PendingOp::None;
	m_pendingState = nullptr;
}

void StateManager::handleEvent(const SDL_Event& e) {
	if (!m_states.empty())
		m_states.top()->handleEvent(e);
}

void StateManager::update() {
	applyPending();
	if (!m_states.empty())
		m_states.top()->update();
}

void StateManager::render() {
	if (!m_states.empty())
		m_states.top()->render();
}