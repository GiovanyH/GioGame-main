#include "material.hpp"

typedef struct {
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    material_t* material;
} mesh_t;

std::vector<float> load_vertices(aiMesh *assimp_mesh) {
    std::vector<float> vertices;
    for (unsigned int i = 0; i < assimp_mesh->mNumVertices; i++) {
        vertices.push_back(assimp_mesh->mVertices[i].x);
        vertices.push_back(assimp_mesh->mVertices[i].y);
        vertices.push_back(assimp_mesh->mVertices[i].z);

        if (assimp_mesh->HasNormals()) {
            vertices.push_back(assimp_mesh->mNormals[i].x);
            vertices.push_back(assimp_mesh->mNormals[i].y);
            vertices.push_back(assimp_mesh->mNormals[i].z);
        }

        if (assimp_mesh->mTextureCoords[0]) {
            vertices.push_back(assimp_mesh->mTextureCoords[0][i].x);
            vertices.push_back(assimp_mesh->mTextureCoords[0][i].y);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }
    return vertices;
}

// returns {VBO, VAO}
std::vector<unsigned int> generate_vertex_buffers(const std::vector<float>& vertices) {
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    int stride = (3 + 3 + 2) * sizeof(float); // Position + Normal + TexCoords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return {VBO, VAO};
}

glm::mat4 aiMatrixToGLM(const aiMatrix4x4 &from) {
    return glm::mat4(
        from.a1, from.b1, from.c1, from.d1,
        from.a2, from.b2, from.c2, from.d2,
        from.a3, from.b3, from.c3, from.d3,
        from.a4, from.b4, from.c4, from.d4
    );
}

mesh_t *create_mesh(aiMesh *assimp_mesh, aiMaterial* material, const std::string& path) {
    mesh_t *mesh = new mesh_t;

    // Load vertices from Assimp
    mesh->vertices = load_vertices(assimp_mesh);

    // Convert path to use forward slashes
    std::string directory = path.substr(0, path.find_last_of('/'));
    std::replace(directory.begin(), directory.end(), '\\', '/');

    aiString str;
    material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

    std::string filename = std::string(str.C_Str());

    if (!filename.empty()) {
        std::replace(filename.begin(), filename.end(), '\\', '/');
        std::string texture_path = directory + '/' + filename;
        
        unsigned int texture_id = texture_id_from_file(texture_path);
        if (texture_id) mesh->material = create_material(texture_id);
        else mesh->material = create_material();
    }
    else {
        mesh->material = create_material();
    }

    // Generate vertex buffers for the loaded vertices
    std::vector<unsigned int> vertex_buffers = generate_vertex_buffers(mesh->vertices);
    mesh->VBO = vertex_buffers[0];
    mesh->VAO = vertex_buffers[1];

    get_assimp_material_properties(material, mesh->material);

    return mesh;
}
