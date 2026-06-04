/*!
	\file StateManager.cpp
	\brief Implementation of StateManager

	Deferred transition logic ensures the state stack is never modified
	during active \c handleEvent() or \c render() calls.
*/

#include "StateManager.h"

//! Sets \c m_pendingOp to \c Push and stores \a state in \c m_pendingState
void StateManager::push(std::unique_ptr<GameStateBase> state) {
	m_pendingOp = PendingOp::Push;
	m_pendingState = std::move(state);
}

//! Sets \c m_pendingOp to \c Pop
void StateManager::pop() {
	m_pendingOp = PendingOp::Pop;
}

//! Sets \c m_pendingOp to \c Replace and stores \a state in \c m_pendingState
void StateManager::replace(std::unique_ptr<GameStateBase> state) {
	m_pendingOp = PendingOp::Replace;
	m_pendingState = std::move(state);
}

/*!
	Calls \c onExit() on the outgoing state and \c onEnter() on the incoming
	one for all transition types. For Push and Replace, also sets
	\c m_manager on the incoming state before \c onEnter(). Resets
	\c m_pendingOp and \c m_pendingState when done.
*/
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

//! Forwards to \c m_states.top()->handleEvent(); no-op if empty
void StateManager::handleEvent(const SDL_Event& e) {
	if (!m_states.empty())
		m_states.top()->handleEvent(e);
}

//! Calls \c applyPending() first, then forwards to \c m_states.top()->update()
void StateManager::update() {
	applyPending();
	if (!m_states.empty())
		m_states.top()->update();
}

//! Forwards to \c m_states.top()->render(); no-op if empty
void StateManager::render() {
	if (!m_states.empty())
		m_states.top()->render();
}