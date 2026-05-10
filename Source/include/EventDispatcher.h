#pragma once
#include "GameEvents.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

class EventDispatcher {
public:
	template<typename T>
	void subscribe(std::function<void(const T&)> cb) {
		m_listeners[typeid(T)].push_back(
			[cb](const GameEventBase& e) {
				cb(static_cast<const T&>(e));
			}
		);
	}

	template<typename T>
	void emit(const T& event) {
		auto it = m_listeners.find(typeid(T));
		if (it != m_listeners.end())
			for (auto& cb : it->second)
				cb(event);
	}

private:
	using Handler = std::function<void(const GameEventBase&)>;
	std::unordered_map<std::type_index, std::vector<Handler>> m_listeners;
};