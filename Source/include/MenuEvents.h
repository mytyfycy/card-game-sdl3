#pragma once
#include "BaseEvent.h"

/*!
    \struct EventButtonHovered
    \brief Emitted when the mouse moves over a menu button
*/
struct EventButtonHovered : EventBase {
	int index; //!< Index of the hovered button, or -1 if none
};
