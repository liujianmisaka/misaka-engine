#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer2D.h"
#include "Components.h"
#include "Entity.h"

namespace Hazel {

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(static_cast<entt::entity>(entity));
    }

    void Scene::OnUpdateRuntime(Timestep ts) {

        // Update Script
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
                // TODO: Move to Scene::OnScenePlay
                if(!nsc.Instance) {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->GetEntity() = Entity{ entity, this };
                    nsc.Instance->OnCreate();
                }
                nsc.Instance->OnUpdate(ts);
                                                          });
        }

        // Render 2D
        const Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform {};
        {
            const auto view = m_Registry.view<CameraComponent, TransformComponent>();
            for(const auto entity : view) {
                auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
                if(camera.Primary) {
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }

        if (mainCamera) {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);
            const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (const auto entity : group) {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                //Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
            }
            Renderer2D::EndScene();
        }
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera) {
        Renderer2D::BeginScene(camera);
        const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (const auto entity : group) {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
        }
        Renderer2D::EndScene();
    }

    void Scene::OnViewportResize(const uint32_t width, const uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize non-FixedAspectRatio Cameras
        const auto view = m_Registry.view<CameraComponent>();
        for (const auto entity : view) {
            auto& cameraComponent = m_Registry.get<CameraComponent>(entity);
            if(!cameraComponent.FixedAspectRatio) {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }

    Entity Scene::GetPrimaryCameraEntity() {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.Primary) {
                return Entity{ entity, this };
            }
        }
        return {};
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T& component) {
        static_assert(false);
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {

    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {

    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {

    }

    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {

    }
}
