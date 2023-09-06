#include "hzpch.h"
#include "Model.h"

#include <assimp/postprocess.h>

namespace Hazel
{
    Model::Model(std::string path)
        : m_ModelPath(std::move(path))
    {
        LoadModel();
        ConvertToHazelData();
    }

    void Model::LoadModel() {
        Assimp::Importer importer;

        // load FBX model
        const aiScene* scene = importer.ReadFile(m_ModelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            HZ_CORE_ERROR("Failed to load model: {}", importer.GetErrorString());
        }

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Data.push_back(processMesh(mesh, scene));
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    MeshData Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertex vertex;

            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoord = { mesh->mTextureCoords[0][i].x,  mesh->mTextureCoords[0][i].x };
            }
            else
            {
                vertex.TexCoord = glm::vec2(0.0f, 0.0f);
            }
            if (mesh->HasTangentsAndBitangents())
            {
                vertex.Tangent = { mesh->mTangents[i].x , mesh->mTangents[i].y, mesh->mTangents[i].z };
                vertex.Bitangent = { mesh->mBitangents[i].x , mesh->mBitangents[i].y, mesh->mBitangents[i].z };
            }

            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return MeshData{ vertices, indices };
    }

    void Model::ConvertToHazelData()
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        uint32_t count = 0;

        for(size_t i = 0; i < m_Data.size(); i++)
        {
            const MeshData& data = m_Data[i];
            for(MeshVertex vertex : data.vertices)
            {
                vertices.push_back(vertex);
            }
            for(uint32_t i = 0; i < data.indices.size(); i++)
            {
                indices.push_back(i + count);
            }
            count = static_cast<uint32_t>(vertices.size());
        }

        m_MeshData = MeshData{ vertices, indices };
    }
}
