#pragma once

namespace Hazel
{
    struct MeshVertex
    {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec4 color {0.2f, 0.4f, 0.8f, 1.0f};
    };

    class Model
    {
    public:
        void LoadModel();

        std::vector<MeshVertex> GetMesh() const { return m_Meshes; }
    private:
        std::string m_ModelPath = "assets/stylized-popcorn-machine-lowpoly/source/SM_Popcorn Machine.fbx";
        std::vector<MeshVertex> m_Meshes {};
    };

}

