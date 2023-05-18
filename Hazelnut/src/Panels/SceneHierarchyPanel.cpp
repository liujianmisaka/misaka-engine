#include "SceneHierarchyPanel.h"

#include <glm/gtc/type_ptr.hpp>
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
            const Entity entity{ entityID, m_Context.get() };
            DrawEntityNode(entity);
        });

        if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectionEntity = {};
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if(m_SelectionEntity) {
            DrawComponents(m_SelectionEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        const auto& tag = entity.GetComponent<TagComponent>().Tag;

        const ImGuiTreeNodeFlags flags = (m_SelectionEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        const bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

        if(ImGui::IsItemClicked()) {
            m_SelectionEntity = entity;
        }

        if(opened) {
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity) {
        if(entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256] = {};
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if(ImGui::InputText("Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }

        if(entity.HasComponent<TransformComponent>()) {
            if(ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;
                // WHY: Where set Camera move style?
                // NOTE: Camera move style is set in Render2D::BeginScene()
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.10f);

                ImGui::TreePop();
            }
        }
    }
}
