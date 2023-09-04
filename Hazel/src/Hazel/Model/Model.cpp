#include "hzpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Hazel
{
    Model::Model(std::string path)
        : m_ModelPath(std::move(path))
    {
        LoadModel();
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

        for(uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertex vertex;

            vertex.m_Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.m_Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            if(mesh->mTextureCoords[0])
            {
                vertex.m_TexCoord = { mesh->mTextureCoords[0][i].x,  mesh->mTextureCoords[0][i].x };
            }
            else
            {
                vertex.m_TexCoord = glm::vec2(0.0f, 0.0f);
            }

            vertex.m_Tangent = { mesh->mTangents[i].x , mesh->mTangents[i].y, mesh->mTangents[i].z };
            vertex.m_Bitangent = { mesh->mBitangents[i].x , mesh->mBitangents[i].y, mesh->mBitangents[i].z };

            vertices.push_back(vertex);
        }

        for(uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for(uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return MeshData{ vertices, indices };
    }
}
