#pragma once

#include "Entity.h"

namespace Hazel {
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() = default;

        template<typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

        Entity& GetEntity() { return m_Entity; }
    public:
        virtual void OnCreate() { }
        virtual void OnDestroy() { }
        virtual void OnUpdate(Timestep) { }
    private:
        Entity m_Entity;

        friend class Entity;
    };
}
