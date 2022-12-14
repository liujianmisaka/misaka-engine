#pragma once

#include "Event.h"

namespace Hazel {
	class HAZEL_API KeyEvent : public Event {
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

	protected:
		KeyEvent(int keycode)
			:m_KeyCode(keycode) {
		}

		int m_KeyCode;
	};


	class HAZEL_API KeyPressEvent : public KeyEvent {
	public:
		KeyPressEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {
		}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		/*	EVENT_CLASS_TYPE(KeyPressed) equal the follow three lines:
				static EventType GetStaticType() { return EventType::KeyPressed; }
				virtual EventType GetEventType() const override { return GetStaticType(); }
				virtual const char* GetName() const override { return "KeyPressed"; }

			事件类型永远是该类的事件，所以有一个静态成员就可以了。
			可能会用到多态，所以需要虚函数 GetEventType() GetName()。
		*/
		static EventType GetStaticType() { return EventType::KeyPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "KeyPressed"; }


	private:
		int m_RepeatCount;
	};


	class HAZEL_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {
		}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "KeyReleased"; }
	};
}