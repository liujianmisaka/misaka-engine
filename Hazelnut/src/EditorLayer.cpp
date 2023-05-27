#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Math/Math.h"

namespace Hazel {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) { }

    void EditorLayer::OnAttach() {
        HZ_PROFILE_FUNCTION();

        m_CheckboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

        FramebufferSpecification fspec;
        fspec.Width = 1280;
        fspec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fspec);

        m_ActiveScene = CreateRef<Scene>();
        #if 0
        // Entity
        m_SquareEntity = m_ActiveScene->CreateEntity("Green Square");
        m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

        auto redSquare = m_ActiveScene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
        m_CameraEntity.AddComponent<CameraComponent>();

        m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
        m_SecondCamera.AddComponent<CameraComponent>().Primary = false;

        class CameraController : public ScriptableEntity {
        public:
            void OnCreate() override {
                auto& translation = GetComponent<TransformComponent>().Translation;
                translation.x = rand() % 10 - 5.0f;
            }

            void OnDestroy() override {

            }

            void OnUpdate(Timestep ts) override {
                auto& translation = GetComponent<TransformComponent>().Translation;
                constexpr float speed = 5.0f;

                if (Input::IsKeyPressed(KeyCode::A))
                    translation.x -= speed * ts;
                if (Input::IsKeyPressed(KeyCode::D))
                    translation.x += speed * ts;
                if (Input::IsKeyPressed(KeyCode::W))
                    translation.y += speed * ts;
                if (Input::IsKeyPressed(KeyCode::S))
                    translation.y -= speed * ts;
            }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        #endif

        // Set panel context. Now m_SceneHierarchyPanel is linked to m_ActiveScene.
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach() {
        HZ_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Hazel::Timestep ts) {

        HZ_PROFILE_FUNCTION();

        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        // Update
        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        // Render
        Renderer2D::ResetStats();    // reset stats
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();


        // Update Scene
        m_ActiveScene->OnuUpdate(ts);

        m_Framebuffer->Unbind();

    }

    void EditorLayer::OnImGuiRender() {
        HZ_PROFILE_FUNCTION();

        // Note: Switch this to true to enable dockspace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
        bool opt_fullscreen = opt_fullscreen_persistant;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Dockspace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize.x = 650.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // Disabling fullscreen would allow the window to be moved to the front of other windows, 
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    NewScene();
                }

                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    OpenScene();
                }

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                    SaveSceneAs();
                }

                if (ImGui::MenuItem("Exit"))   Hazel::Application::Get().Close();
                ImGui::EndMenu();
            }


            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImGuiRender();

        ImGui::Begin("Stats");
        auto stats = Hazel::Renderer2D::GetStats();
        ImGui::Text("Render2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        // TODO: Set when hold entity events
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        //HZ_WARN("Viewport Size: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // Gizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Camera
            auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            const glm::mat4& cameraProjection = camera.GetProjection();
            glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f;
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(
                glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr
            );

            if (ImGuizmo::IsUsing()) {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Hazel::Event& e) {
        m_CameraController.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode()) {
            case Key::N:
            {
                if (control)
                    NewScene();
                break;
            }
            case Key::O:
            {
                if (control)
                    OpenScene();

                break;
            }
            case Key::S:
            {
                if (control && shift)
                    SaveSceneAs();

                break;
            }
            // Gizmos
            case Key::Q:
                m_GizmoType = -1;
                break;
            case Key::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case Key::E:
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            case Key::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            default:
                return false;
        }



        return true;
    }

    void EditorLayer::NewScene() {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene() {
        std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
        if (!filepath.empty()) {
            m_ActiveScene = CreateRef<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);

            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(filepath);
        }
    }

    void EditorLayer::SaveSceneAs() {
        std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
        if (!filepath.empty()) {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(filepath);
        }
    }
}
