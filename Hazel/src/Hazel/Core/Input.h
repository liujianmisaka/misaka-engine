#pragma once

#include "Hazel/Core/Base.h"


namespace Hazel {
	class HAZEL_API Input {
	protected:
        Input() = default;
	public:
        virtual ~Input() = default;

        // 输入事件不可被复制
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;

		static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		
		static std::pair<double, double> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

		static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		
		static float GetMouseY() { return s_Instance->GetMouseYImpl(); }


	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<double, double> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* s_Instance;
	};
}