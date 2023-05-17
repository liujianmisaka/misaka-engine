#pragma once

#include "Entity.h"

namespace Hazel {
    class ScriptableEntity {
    public:
        template<typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

        Entity& GetEntity() { return m_Entity; }
    private:
        Entity m_Entity;

        friend class Entity;
    };
}
