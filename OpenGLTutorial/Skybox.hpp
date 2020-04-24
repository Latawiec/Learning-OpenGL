#pragma once

#include "Utils.hpp"
#include "ShaderProgram.hpp"
#include "stb_image_proxy.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

class Skybox {
    constexpr static inline auto viewName = std::string_view("view");
    constexpr static inline auto projectionName = std::string_view("projection");
    constexpr static inline auto textureName = std::string_view("cubemap");
    unsigned int _texture;
    unsigned int _cubeVAO, _cubeVBO;
    ShaderProgram _program;
    glm::mat4 _view;
    glm::mat4 _projection;
public:
    Skybox(const std::vector<std::string>& textureFilepaths) {

        glGenVertexArrays(1, &_cubeVAO);
        glGenBuffers(1, &_cubeVBO);

        glBindVertexArray(_cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

        if (auto error = glGetError(); error != GL_NO_ERROR) {
			std::cout << "Error: " << error << '\n';
		}

        int width, height, nrChannels, i = 0;
        unsigned char* data;
        for(const auto& filepath : textureFilepaths) {
            data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
            if (!data) {
                throw std::runtime_error("Failed to load texture file.");
            }
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            i++;
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        const std::string vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/Skybox/" "Skybox.vert.glsl");
        const std::string fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/Skybox/" "Skybox.frag.glsl");

        if (auto error = glGetError(); error != GL_NO_ERROR) {
			std::cout << "Error: " << error << '\n';
		}

        _program = {
            Shader<ShaderType::Vertex>(vertexShaderCode),
            Shader<ShaderType::Fragment>(fragmentShaderCode),
        };
    }

    void updateTransform(const glm::mat4& view, const glm::mat4& projection) {
        _view = glm::mat4(glm::mat3(view));
        _projection = projection;
    }

    void draw() {
        glDepthMask(GL_FALSE);
        _program.use();
        _program.set(viewName, _view);
        _program.set(projectionName, _projection);
        glBindVertexArray(_cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }

private:
    constexpr static float cubeVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
};