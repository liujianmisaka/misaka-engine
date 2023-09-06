#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Hazel/Renderer/VertexStruct.h"

namespace Hazel
{
    struct MeshData
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;
    };

    class Model
    {
    public:
        Model(std::string path);
        const MeshData& GetData() { return m_MeshData; }
        
    private:
        void LoadModel();
        void processNode(aiNode* node, const aiScene* scene);
        MeshData processMesh(aiMesh* mesh, const aiScene* scene);

        void ConvertToHazelData();
    private:
        std::string m_ModelPath;
        std::vector<MeshData> m_Data;
        MeshData m_MeshData;
    };

}

