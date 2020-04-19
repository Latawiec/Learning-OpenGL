#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "VertexData.hpp"
#include "ShaderProgram.hpp"

class BayerMatrixDither {
    constexpr static auto processedTextureName = std::string_view("imageTexture");
    constexpr static auto matrixTextureName = std::string_view("bayerMatrixTexture");
    constexpr static auto matrixVerticalScaleName = std::string_view("matrixVerticalScale");
    constexpr static auto matrixHorizontalScaleName = std::string_view("matrixHorizontalScale");
    
    unsigned int _matrixTexture;
    VertexDataBase _vertexData;
    ShaderProgram _shaderProgram;
public:
    BayerMatrixDither() {
        auto indicesVector = std::vector<unsigned int>(indices, indices + 6);
        _vertexData = VertexData<Layout::Sequential, Vec3>(indicesVector, 4, reinterpret_cast<const float*>(quadCoords));

        const auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "BayerDither.vert.glsl");
	    const auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "BayerDither.frag.glsl");

        _shaderProgram = ShaderProgram(
            Shader<ShaderType::Vertex>(vertexShaderCode.c_str()),
            Shader<ShaderType::Fragment>(fragmentShaderCode.c_str())
        );

        glGenTextures(1, &_matrixTexture);
        glBindTexture(GL_TEXTURE_2D, _matrixTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, bayer_4_4);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw(unsigned int texture) {
        _shaderProgram.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        _shaderProgram.set(processedTextureName, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _matrixTexture);
        _shaderProgram.set(matrixTextureName, 1);

        glActiveTexture(GL_TEXTURE0);
        auto vertexBinding = VertexDataBase::ScopedBinding(_vertexData);
        glDrawElements(GL_TRIANGLES, _vertexData.vertexCount(), GL_UNSIGNED_INT, 0);
    }

    void setMatrixDensity(int width, int height) {
        _shaderProgram.use();
        _shaderProgram.set(matrixVerticalScaleName, static_cast<float>(height) / 4.f);
        _shaderProgram.set(matrixHorizontalScaleName, static_cast<float>(width) / 4.f);
    }

    constexpr static inline glm::vec3 bayer_2_2[4] = {
        glm::vec3(0.f), glm::vec3(0.5f),
        glm::vec3(0.75f), glm::vec3(0.25f),
    };

    constexpr static inline glm::vec3 bayer_4_4[16] = {
        glm::vec3(float(0)/float(16)),  glm::vec3(float(8)/float(16)),  glm::vec3(float(2)/float(16)),  glm::vec3(float(10)/float(16)),
        glm::vec3(float(12)/float(16)), glm::vec3(float(4)/float(16)),  glm::vec3(float(14)/float(16)), glm::vec3(float(6)/float(16)),
        glm::vec3(float(3)/float(16)),  glm::vec3(float(11)/float(16)), glm::vec3(float(1)/float(16)),  glm::vec3(float(9)/float(16)),
        glm::vec3(float(15)/float(16)), glm::vec3(float(7)/float(16)),  glm::vec3(float(13)/float(16)), glm::vec3(float(5)/float(16)),
    };

    constexpr static inline glm::vec3 quadCoords[4] = {
        {-1.f, -1.f, 0.f},
        {1.f, -1.f, 0.f},
        {1.f, 1.f, 0.f},
        {-1.f, 1.f, 0.f}
    };

    constexpr static inline unsigned int indices[6] = {
        0, 1, 2, 2, 0, 3
    };

};