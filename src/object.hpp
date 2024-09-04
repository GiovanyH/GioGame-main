#include "node.hpp"

#include <map>
#include <string>
#include <vector>

#define ASSIMP_LOAD_FLAGS \
    ( aiProcess_Triangulate \
    | aiProcess_CalcTangentSpace \
    | aiProcess_TransformUVCoords \
    | aiProcess_FindDegenerates \
    | aiProcess_GenNormals \
    | aiProcess_RemoveRedundantMaterials \
    | aiProcess_ImproveCacheLocality \
    | aiProcess_SplitLargeMeshes \
    | aiProcess_LimitBoneWeights \
    | aiProcess_FindInvalidData \
    | aiProcess_FindInstances \
    | aiProcess_SortByPType \
    | aiProcess_EmbedTextures )

#define MAX_NUM_BONES_PER_VERTEX 4
#define ARRAY_SIZE_IN_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))
struct vertex_bone_data {
    int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

    vertex_bone_data() {

    }

    void add_bone_data(int BoneID, float Weight) {
        for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++) {
            if (Weights[i] == 0.0f) {
                BoneIDs[i] = BoneID;
                Weights[i] = Weight;
                printf("bone %d weight %f index %i\n", BoneID, Weight, i);
                return;
            }
        }

        assert(0);
    }
};

std::vector<vertex_bone_data> vertex_to_bones;
std::vector<int> mesh_base_vertex;
std::map<std::string, int> bone_name_to_index_map;

int get_bone_id(const aiBone *bone) {
    int bone_id = 0;
    std::string bone_name(bone->mName.C_Str());

    if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) {
        bone_id = bone_name_to_index_map.size();
        bone_name_to_index_map[bone_name] = bone_id;
    }
    else {
        bone_id = bone_name_to_index_map[bone_name];
    }

    return bone_id;
}

void parse_single_bone(int mesh_index, const aiBone *bone) {
    int bone_id = get_bone_id(bone);
    //std::cout << "bone id " << bone_id << std::endl;

    for (int i = 0; i < bone->mNumWeights; i++) {
        if (i == 0) std::cout << std::endl;
        const aiVertexWeight& vw = bone->mWeights[i];

        int global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
        //std::cout << "Vertex id " << global_vertex_id << ' ';

        assert(global_vertex_id < vertex_to_bones.size());
        vertex_to_bones[global_vertex_id].add_bone_data(bone_id, vw.mWeight);
    }

    //std::cout << std::endl;
}

void parse_mesh_bones(int mesh_index, const aiMesh *mesh) {
    for (int i = 0; i < mesh->mNumBones; i++) {
        parse_single_bone(mesh_index, mesh->mBones[i]);
    }
}

void parse_meshes(const aiScene *scene) {
    std::cout << "==========================================" << std::endl;
    std::cout << "Parsing " << scene->mNumMeshes << " meshes" << std::endl;

    unsigned int total_vertices = 0;
    unsigned int total_indices = 0;
    unsigned int total_bones = 0;

    mesh_base_vertex.resize(scene->mNumMeshes);

    for (int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh *mesh = scene->mMeshes[i];
        unsigned int num_vertices = mesh->mNumVertices;
        unsigned int num_indices = mesh->mNumFaces * 3;
        unsigned int num_bones = mesh->mNumBones;
        mesh_base_vertex[i] = total_vertices;
        std::cout << "Mesh " << i << ' ' << mesh->mName.C_Str() << ": " << "vertices " << num_vertices << " indices " << num_indices << " bones " << num_bones << std::endl;
        total_vertices += num_vertices;
        total_indices += num_indices;
        total_bones += num_bones;

        vertex_to_bones.resize(total_vertices);

        if (mesh->HasBones()) {
            parse_mesh_bones(i, mesh);
        }

        std::cout << std::endl;
    }

    std::cout << "Object: " << "total vertices " << total_vertices << " total indices " << total_indices << " total bones " << total_bones << std::endl;
}

void parse_assimp_scene(const aiScene *scene) {
    parse_meshes(scene);
}

node *create_object(std::string mesh_path) {
    node *object = new node();
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(mesh_path, ASSIMP_LOAD_FLAGS);
    if (!scene) {
        std::cout << "Error parsing ASSIMP scene -> object.hpp line 9" << std::endl;
    }

    parse_assimp_scene(scene);
    
    glm::mat4 node_transform = aiMatrixToGLM(scene->mRootNode->mTransformation);

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        object->add_geometry(create_mesh(mesh, material, mesh_path));
    }

    object->set_transformation(node_transform);

    return object;
}
