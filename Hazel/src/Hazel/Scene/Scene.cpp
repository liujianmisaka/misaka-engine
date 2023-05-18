#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer2D.h"
#include "Component.h"
#include "Entity.h"

namespace Hazel {

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::OnuUpdate(Timestep ts) {

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
        const glm::mat4* cameraTransform = nullptr;
        {
            const auto view = m_Registry.view<CameraComponent, TransformComponent>();
            for(const auto entity : view) {
                auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
                if(camera.Primary) {
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.Transform;
                    break;
                }
            }
        }

        if(mainCamera) {
            Renderer2D::BeginScene(*mainCamera, *cameraTransform);
            const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (const auto entity : group) {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad(transform, sprite.Color);
            }
            Renderer2D::EndScene();
        }

    }

    void Scene::OnViewportResize(const uint32_t width, const uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize non-FixedAspectRatio Cameras
        const auto view = m_Registry.view<CameraComponent>();
        for(const auto entity : view) {
            auto& cameraComponent = m_Registry.get<CameraComponent>(entity);
            if(!cameraComponent.FixedAspectRatio) {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }
}
