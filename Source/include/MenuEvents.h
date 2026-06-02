#pragma once
#include "BaseEvent.h"

// Najechano myszka na guzik
struct EventButtonHovered : EventBase {
	int index;
};
