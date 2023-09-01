#include "hzpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void Model::LoadModel() {
    Assimp::Importer importer;

    // load FBX model
    const aiScene* scene = importer.ReadFile(m_ModelPath,
                                             aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    HZ_CORE_INFO("fbx mesh number: {}", scene[0].mNumMeshes);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        HZ_CORE_ERROR("Failed to load model: {}", importer.GetErrorString());
    }
}
