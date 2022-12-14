#include "hzpch.h"

#include <GLFW/glfw3.h>

#include "Application.h"
#include "Hazel/Log.h"

namespace Hazel {
	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		HZ_CORE_TRACE("{0}", e);
	}

	Application::~Application() {

	}

	void Application::Run() {

		while (m_Running) {
			glClearColor(1, 0, 1, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowClosedEvent& e) {
		m_Running = false;
		return true;
	}
}