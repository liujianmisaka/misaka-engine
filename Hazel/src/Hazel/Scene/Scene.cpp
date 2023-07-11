#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Entity.h"
#include "ScriptableEntity.h"

namespace Hazel {

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType) {
        switch (bodyType) {
            case Rigidbody2DComponent::BodyType::Static:       return b2BodyType::b2_staticBody;
            case Rigidbody2DComponent::BodyType::Dynamic:      return b2BodyType::b2_dynamicBody;
            case Rigidbody2DComponent::BodyType::Kinematic:    return b2BodyType::b2_kinematicBody;
        }
        HZ_CORE_ASSERT(false, "Unknown body type");
        return b2BodyType::b2_staticBody;
    }

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>();
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(static_cast<entt::entity>(entity));
    }

    void Scene::OnRuntimeStart() {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for(auto e : view) {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;

            if(entity.HasComponent<BoxCollider2DComponent>()) {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::OnRuntimeStop() {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
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

        // Physics 2D
        {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for(auto e : view) {
                Entity entity = { e, this };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto rb2d = entity.GetComponent<Rigidbody2DComponent>();


                b2Body* body = static_cast<b2Body*>(rb2d.RuntimeBody);
                const auto& postion = body->GetPosition();
                transform.Translation.x = postion.x;
                transform.Translation.y = postion.y;
                transform.Rotation.z = body->GetAngle();
            }
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
        //static_assert(false);
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
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

    template<>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {

    }

    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {

    }
}
