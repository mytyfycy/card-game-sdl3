#pragma once
#include "EventDispatcher.h"

/*!
	\class Subscribable
	\brief Mixin that exposes a public \c subscribe() interface over an internal EventDispatcher

	Classes that inherit from \c Subscribable (e.g. \c Game, \c MainMenu) allow
	external systems to register event callbacks without exposing the dispatcher directly.
	Emitting events remains the responsibility of the inheriting class via \c m_dispatcher.
*/
class Subscribable {
public:
	/*!
		\brief Registers a callback for events of type \c T
		\tparam T   Event type, must derive from \c EventBase
		\param  cb  Callback invoked when an event of type \c T is emitted
	*/
	template<typename T>
	void subscribe(std::function<void(const T&)> cb) {
		m_dispatcher.subscribe<T>(cb);
	}

protected:
	//! Internal event dispatcher; used by the inheriting class to emit events
	EventDispatcher m_dispatcher;
};