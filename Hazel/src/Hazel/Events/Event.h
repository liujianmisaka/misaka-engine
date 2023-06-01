#pragma once

#include "hzpch.h"

#include "Hazel/Core/Base.h"

namespace Hazel {

    // Events in Hazel are currently blocking, meaning when an event occurs it
    // immediately gets dispatched and must be dealt with right then an there.
    // For the future, a better strategy might be to buffer events in an event
    // bus and process them during the "event" part of the update stage.

	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4),
	};


	class Event {
	public:
        virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
	};

	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event)
			: m_Event(event) { }

		template<typename T>
		//bool Dispatch(EventFn<T> func) {    // bool func(T& e);
		bool Dispatch(std::function<bool(T&)> func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
			    // m_Event会使用Event类型初始化
				// 取m_Event的地址，然后强制转换为T* 类型的指针，再解引用获得T类型的对象
				// Handled = func(event);
				m_Event.Handled = func(*static_cast<T*>(&m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;

	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}
}