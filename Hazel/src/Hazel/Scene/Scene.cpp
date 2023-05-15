#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer2D.h"
#include "Component.h"
#include "Entity.h"

namespace Hazel {

    static void OnTransformConstruct(entt::registry& registry, entt::entity& entity) {
        
    }

    Scene::Scene() {
#ifdef ENTT_EXAMPLE_CODE
        struct MeshComponent {
            glm::vec2 mesh;
        };

        struct TransformComponent {
            glm::mat4 Transform{ 1.0f };

            TransformComponent() = default;
            TransformComponent(const TransformComponent&) = default;
            TransformComponent(const glm::mat4& transform)
                : Transform(transform) { }

            operator glm::mat4& () { return Transform; }
            operator const glm::mat4& () const { return Transform; }
        };

        auto entity = m_Registry.create();
        m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

        m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

        if(m_Registry.all_of<TransformComponent>(entity)) {
            TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
        }

        auto view = m_Registry.view<TransformComponent>();
        for (auto entity : view) {
            TransformComponent& transform = view.get<TransformComponent>(entity);
        }

        auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
        for (auto entity : group) {
            auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
        }
#endif
    }

    Scene::~Scene() {}

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::OnuUpdate(Timestep ts) {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite.Color);
        }
    }
}
