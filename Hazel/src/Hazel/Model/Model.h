#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Hazel/Renderer/RendererData.h"

namespace Hazel
{
    class Model
    {
    public:
        Model(std::string path);
        const std::vector<MeshData>& GetData() { return m_Data; }

    private:
        void LoadModel();
        void processNode(aiNode* node, const aiScene* scene);
        MeshData processMesh(aiMesh* mesh, const aiScene* scene);
    private:
        std::string m_ModelPath;
        std::vector<MeshData> m_Data;
    };

}

