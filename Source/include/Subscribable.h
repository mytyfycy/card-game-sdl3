#pragma once
#include "EventDispatcher.h"

class Subscribable {
public:
	template<typename T>
	void subscribe(std::function<void(const T&)> cb) {
		m_dispatcher.subscribe<T>(cb);
	}

protected:
	EventDispatcher m_dispatcher;
};