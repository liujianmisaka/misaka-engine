#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"


namespace Hazel {
	class HAZEL_API Input {
	public:
        static bool IsKeyPressed(KeyCode keycode);
        static bool IsMouseButtonPressed(MouseCode button);
        static std::pair<double, double> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
	};
}