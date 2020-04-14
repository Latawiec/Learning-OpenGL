#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>
#include <string>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ShaderProgram.hpp"

enum class TextureType : uint8_t {
    Diffuse = 0,
    Specular,
    Normal,
    SIZE
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture {
    unsigned int id;
    TextureType type;

    Texture(std::string_view filepath, TextureType type) : type(type) {
        
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filepath.data(), &width, &height, &nrChannels, 0);
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    operator unsigned int() {
        return id;
    }
};

class Mesh {
    constexpr static auto texturesAccessPrefix = std::string_view("material.");
    constexpr static auto diffuseMapArrayName = std::string_view("diffuseTextures");
    constexpr static auto specularMapArrayName = std::string_view("specularTextures");
    constexpr static auto normalMapArrayName = std::string_view("normalTextures");

    constexpr static std::string_view mapTextureTypeToName(TextureType type) {
        switch (type) {
            case TextureType::Diffuse:
                return diffuseMapArrayName;
            case TextureType::Specular:
                return specularMapArrayName;
            case TextureType::Normal:
                return normalMapArrayName;
            default:
                throw std::runtime_error("Not valid type.");
        }
    }

    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    unsigned int VAO, VBO, EBO;
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
        _vertices(std::move(vertices)),
        _indices(std::move(indices)),
        _textures(std::move(textures)) {
            setupMesh();
        }

    void Draw(ShaderProgram& shader) {
        std::array<int, static_cast<size_t>(TextureType::SIZE)> textureCounters{};
        
        for (int textureIndex=0; textureIndex<_textures.size(); ++textureIndex) {
            const auto& texture = _textures[textureIndex];
            glActiveTexture(GL_TEXTURE0 + textureIndex);

            std::string uniformReference =
                std::string(texturesAccessPrefix) +
                std::string(mapTextureTypeToName(texture.type)) +
                '[' +
                std::to_string(textureCounters[static_cast<int>(texture.type)]) +
                ']';

            shader.set(uniformReference, textureIndex);
            glBindTexture(GL_TEXTURE_2D, _textures[textureIndex].id);
            textureCounters[static_cast<int>(texture.type)]++;
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};