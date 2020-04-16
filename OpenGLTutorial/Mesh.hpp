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
#include "VertexData.hpp"

enum class TextureType : uint8_t {
    Diffuse = 0,
    Specular,
    Normal,
    SIZE
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

    VertexDataBase _vertexData;
    std::vector<Texture> _textures;
public:
    
    Mesh(VertexDataBase vertexData, std::vector<Texture> textures)
    : _vertexData(vertexData),
      _textures(std::move(textures))
    {}

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

        VertexDataBase::ScopedBinding bind(_vertexData);
        glDrawElements(GL_TRIANGLES, _vertexData.elementsCount(), GL_UNSIGNED_INT, 0);
    }
};