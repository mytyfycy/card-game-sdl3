#pragma once
#include <memory>
#include <stack>
#include "GameStateBase.h"

/*!
	\class StateManager
	\brief Manages a stack of GameStateBase instances

	State transitions (push, pop, replace) are deferred until the next
	\c update() call to avoid modifying the stack during event handling
	or rendering. Only the top state receives \c handleEvent(), \c update(),
	and \c render() calls.
*/
class StateManager {
public:
	/*!
		\brief Schedules a new state to be pushed onto the stack
		\param state State to push; becomes active on the next \c update()
	*/
	void push(std::unique_ptr<GameStateBase> state);

	//! Schedules the top state to be popped on the next \c update()
	void pop();

	/*!
		\brief Schedules the top state to be replaced by a new one
		\param state Replacement state; becomes active on the next \c update()
	*/
	void replace(std::unique_ptr<GameStateBase> state);

	//! Forwards the event to the top state; no-op if the stack is empty
	void handleEvent(const SDL_Event& e);

	//! Applies any pending transition, then calls \c update() on the top state
	void update();

	//! Calls \c render() on the top state; no-op if the stack is empty
	void render();

	//! Returns \c true if the state stack is empty
	bool isEmpty() const { return m_states.empty(); }

	//! Returns a raw pointer to the top state, or \c nullptr if the stack is empty
	GameStateBase* getCurrentState() const {
		return m_states.empty() ? nullptr : m_states.top().get();
	}

	/*!
		\brief Returns \c true if the top state is an instance of \c T
		\tparam T State type to check against
	*/
	template <typename T>
	bool isCurrentState() const {
		if (m_states.empty()) return false;
		return dynamic_cast<const T*>(m_states.top().get()) != nullptr;
	}

private:
	std::stack<std::unique_ptr<GameStateBase>> m_states; //!< Stack of active states

	/*!
		\enum PendingOp
		\brief Deferred transition operation to apply on the next \c update()

		\var PendingOp::None     No transition pending
		\var PendingOp::Push     Push \c m_pendingState onto the stack
		\var PendingOp::Pop      Pop the top state
		\var PendingOp::Replace  Replace the top state with \c m_pendingState
	*/
	enum class PendingOp { None, Push, Pop, Replace};

	PendingOp m_pendingOp = PendingOp::None; //!< Transition scheduled for the next update
	std::unique_ptr<GameStateBase> m_pendingState = nullptr; //!< State to push or replace with, if applicable

	//! Executes the pending transition and resets \c m_pendingOp to \c None
	void applyPending();
};