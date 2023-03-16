#pragma once

#include "Hazel/Input.h"

namespace Hazel {
	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int 
		) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<double, double> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}