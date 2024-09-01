#pragma once

#include <glad/glad.h>

#include <iostream>

#include <vector>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "file_manager.hpp"

typedef struct {
    unsigned int texture_id;
    unsigned int shader_program;

    glm::vec4 base_color;
    uint8_t roughness;
    uint8_t metallic;
} material_t;

std::string root_path = "C:/Users/henri/OneDrive/Desktop/Javascript/GioGame";

void get_assimp_material_properties(aiMaterial *assimp_material, material_t *material) {
    // Initialize default values
    material->base_color = glm::vec4(1.0f); // Default to white color
    material->roughness = 0; // Default roughness
    material->metallic = 0; // Default metallic

    // Extract the base color (diffuse color)
    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f); // Default to white
    if (AI_SUCCESS == assimp_material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        material->base_color = glm::vec4(color.r, color.g, color.b, color.a);
    }

    // Extract roughness (using Assimp PBR key if available)
    float roughness = 0.0f;
    if (AI_SUCCESS == assimp_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness)) {
        material->roughness = static_cast<uint8_t>(roughness * 255.0f); // Convert to 0-255 range
    }

    // Extract metallic (using Assimp PBR key if available)
    float metallic = 0.0f;
    if (AI_SUCCESS == assimp_material->Get(AI_MATKEY_METALLIC_FACTOR, metallic)) {
        material->metallic = static_cast<uint8_t>(metallic * 255.0f); // Convert to 0-255 range
    }

    std::cout << "Creatubg styff" << std::endl;
}

unsigned int create_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    unsigned int shader_program;
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

const char *load_default_vertex_shader_source() {
    return load_file_as_string(root_path + "/src/shaders/model.vert");
}

const char *load_default_fragment_shader_source() {
    return load_file_as_string(root_path + "/src/shaders/model.frag");
}

material_t *create_material(
    unsigned int texture_id = 0,
    const char *vertex_shader_source = load_default_vertex_shader_source(), 
    const char *fragment_shader_source = load_default_fragment_shader_source()) 
{
    material_t *material = new material_t;

    unsigned int shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);

    material->texture_id = texture_id;
    material->shader_program = shader_program;

    return material;
}

material_t *create_default_material() {
    material_t *material = new material_t;

    const char *vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char *fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Output white color
        }
    )";

    unsigned int shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);
    material->shader_program = shader_program;

    return material;
}