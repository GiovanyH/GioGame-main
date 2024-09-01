#include "node.hpp"

typedef struct {
    std::vector<mesh_t*> meshes;
    glm::mat4 model;
} object_t;

object_t *create_object(std::string mesh_path) {
    object_t *object = new object_t;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(mesh_path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs);
    
    glm::mat4 nodeTransform = aiMatrixToGLM(scene->mRootNode->mTransformation);

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        object->meshes.push_back(create_mesh(mesh, material, mesh_path));
    }

    object->model = nodeTransform;

    return object;
}
