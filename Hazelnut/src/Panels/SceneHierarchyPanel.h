#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {
    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel() = default;
        explicit SceneHierarchyPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);

        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectionEntity; }

    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> m_Context;
        Entity m_SelectionEntity;
    };
}
