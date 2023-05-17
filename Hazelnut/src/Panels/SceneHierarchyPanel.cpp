#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include "Hazel/Scene/Component.h"

namespace Hazel {
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        ImGui::Begin("Scene Hierarchy");

        m_Context->m_Registry.each([&](auto entityID) {
            Entity entity{ entityID, m_Context.get() };
            DrawEntityNode(entity);
        });

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = (m_SelectionEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        const bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

        if(ImGui::IsItemClicked()) {
            m_SelectionEntity = entity;
        }

        if(opened) {
            ImGui::TreePop();
        }
    }
}
