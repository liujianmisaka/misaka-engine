#include "hzpch.h"

#include "Hazel/Core/Input.h"

#include <GLFW/glfw3.h>

#include "Hazel/Core/Application.h"

namespace Hazel {
	bool Input::IsKeyPressed(KeyCode keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

		return state == GLFW_PRESS;
	}

	std::pair<double, double> Input::GetMousePosition() {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return std::pair<double, double>(xpos, ypos);
	}

	float Input::GetMouseX() {
		auto [x, y] = GetMousePosition();
		return (float)x;
	}

	float Input::GetMouseY() {
		auto [x, y] = GetMousePosition();
		return (float)y;
	}
}