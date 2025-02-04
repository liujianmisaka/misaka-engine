#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

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


    /*
     * for each entt::entity which has the same uuid in src and dst, copy the given Component.
     */
    template<typename Component>
    static void CopyComponent(entt::registry& dst, const entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
        auto view = src.view<Component>();
        for(auto e : view) {
            UUID uuid = src.get<IDComponent>(e).ID;
            HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
            entt::entity dstEnttID = enttMap.at(uuid);

            auto& component = src.get<Component>(e);
            dst.emplace_or_replace<Component>(dstEnttID, component);
        }
    }

    template<typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src) {
        if(src.HasComponent<Component>()) {
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
        }
    }

    Scene::~Scene() {
        delete m_PhysicsWorld;
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other) {
        Ref<Scene> newScene = CreateRef<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for(auto e : idView) {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = static_cast<entt::entity>(newEntity);
        }

        // Copy components (except IDComponent and TagComponent)
        CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

        return  newScene;
    }

    Entity Scene::CreateEntity(const std::string& name) {
        return CreateEntityWithUUID(UUID(), name);
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
        OnPhysics2DStart();
    }

    void Scene::OnRuntimeStop() {
        OnPhysics2DStop();
    }

    void Scene::OnSimulationStart() {
        OnPhysics2DStart();
    }

    void Scene::OnSimulationStop() {
        OnPhysics2DStop();
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

            // Draw Sprites
            {
                const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
                for (const auto entity : group) {
                    auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                    //Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
                    Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
                }
            }

            // Draw Circles
            {
                const auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                for(auto entity : view) {
                    auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                    Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
                }
            }
            Renderer2D::EndScene();
        }
    }

    void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera) {
        // Physics 2D
        {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();
            for (auto e : view) {
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

        // Render
        RenderScene(camera);
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera) {
        // Render
        RenderScene(camera);
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

    void Scene::DuplicateEntity(Entity entity) {
        Entity newEntity = CreateEntity(entity.GetName());

        CopyComponentIfExists<TransformComponent>(newEntity, entity);
        CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
        CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
        CopyComponentIfExists<CameraComponent>(newEntity, entity);
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
        CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
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

    void Scene::OnPhysics2DStart() {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view) {
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

            if (entity.HasComponent<BoxCollider2DComponent>()) {
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

            if (entity.HasComponent<CircleCollider2DComponent>()) {
                auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
                circleShape.m_radius = transform.Scale.x * cc2d.Radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = cc2d.Density;
                fixtureDef.friction = cc2d.Friction;
                fixtureDef.restitution = cc2d.Restitution;
                fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::OnPhysics2DStop() {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    void Scene::RenderScene(EditorCamera& camera) {
        Renderer2D::BeginScene(camera);

        // Draw sprites
        {
            const auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (const auto entity : group) {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
                //Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f), static_cast<int>(entity));
            }
        }

        // Draw circle
        {
            const auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
            }
        }

        //Renderer2D::DrawLine(glm::vec3(2.0f), glm::vec3(5.0f), glm::vec4(1, 0, 1, 1));
        //Renderer2D::DrawRect(glm::vec3(0.0f), glm::vec2(1.0f), glm::vec4(1, 1, 1, 1));

        Renderer2D::EndScene();
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
    void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {

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

    template<>
    void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {

    }
}
