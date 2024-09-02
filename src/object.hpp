#include "node.hpp"

node *create_object(std::string mesh_path) {
    node *object = new node();
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(mesh_path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs);
    
    glm::mat4 node_transform = aiMatrixToGLM(scene->mRootNode->mTransformation);

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        object->add_geometry(create_mesh(mesh, material, mesh_path));
        //object->meshes.push_back(create_mesh(mesh, material, mesh_path));
    }

    object->set_transformation(node_transform);

    return object;
}
