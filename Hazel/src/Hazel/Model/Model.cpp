#include "hzpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Hazel
{
    void Model::LoadModel() {
        Assimp::Importer importer;

        // load FBX model
        const aiScene* scene = importer.ReadFile(m_ModelPath,
                                                 aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        for (uint32_t index = 0; index < 1/*scene->mNumMeshes*/; index++) {
            const aiMesh* aimesh = scene->mMeshes[index];
            for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
            {
                MeshVertex meshVertex{};

                //meshVertex.vertex = Utils::aiVector3DToGLm(aimesh->mVertices[i]);
                meshVertex.m_Position = { aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z };
                meshVertex.m_Color = { 0.2f, 0.4f, 0.8f, 1.0f };
                meshVertex.m_Normal = { aimesh->mNormals[i].x,  aimesh->mNormals[i].y, aimesh->mNormals[i].z };

                m_MeshVertices.push_back(meshVertex);
            }
        }

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            HZ_CORE_ERROR("Failed to load model: {}", importer.GetErrorString());
        }
    }
}
