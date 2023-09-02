#pragma once

#include "Hazel/Vertex/MeshVertex.h"

namespace Hazel
{
    class Model
    {
    public:
        void LoadModel();

        std::vector<MeshVertex> GetMesh() const { return m_MeshVertices; }
    private:
        std::string m_ModelPath = "assets/stylized-popcorn-machine-lowpoly/source/SM_Popcorn Machine.fbx";
        std::vector<MeshVertex> m_MeshVertices {};
    };

}

