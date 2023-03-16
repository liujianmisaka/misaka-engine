#include "hzpch.h"

#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Hazel/Application.h"

namespace Hazel {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<double, double> WindowsInput::GetMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return std::pair<double, double>(xpos, ypos);
	}

	float WindowsInput::GetMouseXImpl() {
		auto [x, y] = GetMousePositionImpl();
		return (float)x;
	}

	float WindowsInput::GetMouseYImpl() {
		auto [x, y] = GetMousePositionImpl();
		return (float)y;
	}
}