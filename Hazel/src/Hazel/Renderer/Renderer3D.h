#pragma once

#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Scene/Components.h"

namespace Hazel {
    class MeshVertex;

    class Renderer3D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
        static void EndScene();
        static void Flush();

        static void DrawMesh(const glm::mat4& transform, const std::vector<MeshVertex>& meshes);

        static void StartBatch();
        static void NextBatch();
    };
}
