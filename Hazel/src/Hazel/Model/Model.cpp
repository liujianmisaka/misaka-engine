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

        const aiMesh* aimesh = scene->mMeshes[0];

        for(uint32_t i =0; i < aimesh->mNumVertices; i++)
        {
            MeshVertex mesh{};

            mesh.vertex = { aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z };
            mesh.normal = { aimesh->mNormals[i].x,  aimesh->mNormals[i].y, aimesh->mNormals[i].z };

            m_Meshes.push_back(mesh);
        }

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            HZ_CORE_ERROR("Failed to load model: {}", importer.GetErrorString());
        }
    }
}
