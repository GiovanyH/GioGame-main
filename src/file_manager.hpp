#include "material.hpp"

#include <fstream>
#include <sstream>

// returns an OpenGL TextureID based on an image
unsigned int texture_id_from_file(std::string path) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, texture_id); // Bind the texture to GL_TEXTURE_2D target

    // Load image
    int width, height, nrChannels;
    std::cout << "loading texture from: " << path << std::endl;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 2)
            format = GL_RG;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
            stbi_image_free(data);
            return 0;
        }

        // Upload image data to the GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the texture

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Free image data after uploading
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0; // Return 0 to indicate an error
    }

    return texture_id;
}

const char *load_file_as_string(std::string path) {
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string content = buffer.str();
    char* file_contents = new char[content.size() + 1];
    std::copy(content.begin(), content.end(), file_contents);
    file_contents[content.size()] = '\0';

    return file_contents;
}