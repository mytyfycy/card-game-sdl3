#pragma once
#include "BaseEvent.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

/*!
	\class EventDispatcher
	\brief Type-safe publish/subscribe event bus

	Listeners are registered per event type via \c subscribe() and invoked
	synchronously when \c emit() is called with a matching event instance.
*/
class EventDispatcher {
public:
	/*!
		\brief Registers a callback for events of type \c T
		\tparam T    Event type, must derive from \c EventBase
		\param  cb   Callback invoked with a const reference to the event
	*/
	template<typename T>
	void subscribe(std::function<void(const T&)> cb) {
		m_listeners[typeid(T)].push_back(
			[cb](const EventBase& e) {
				cb(static_cast<const T&>(e));
			}
		);
	}

	/*!
		\brief Dispatches an event to all registered listeners of type \c T
		\tparam T     Event type, must derive from \c EventBase
		\param  event Event instance to dispatch
	*/
	template<typename T>
	void emit(const T& event) {
		auto it = m_listeners.find(typeid(T));
		if (it != m_listeners.end())
			for (auto& cb : it->second)
				cb(event);
	}

private:
	//! Type-erased handler wrapping a typed callback
	using Handler = std::function<void(const EventBase&)>;

	//! Maps each event type to its list of registered handlers
	std::unordered_map<std::type_index, std::vector<Handler>> m_listeners;
};