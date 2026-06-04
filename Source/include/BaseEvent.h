#pragma once

/*!
    \struct EventBase
    \brief Base type for all game and menu events

    All event structs must derive from this to be usable
    with \c EventDispatcher and \c Subscribable.
*/
struct EventBase {
	virtual ~EventBase() = default;
};