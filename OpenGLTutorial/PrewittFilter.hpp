#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "VertexData.hpp"
#include "ShaderProgram.hpp"
#include "Utils.hpp"

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

class PrewittFilter {
    constexpr static auto processedTextureName = std::string_view("imageTexture");
    constexpr static auto prewittVerticalTextureName = std::string_view("prewittVerticalTexture");
    constexpr static auto prewittHorizontalTextureName = std::string_view("prewittHorizontalTexture");
    constexpr static auto kernelSizeName = std::string_view("kernelSize");
    constexpr static auto kernelVerticalScaleName = std::string_view("imageVerticalScale");
    constexpr static auto kernelHorizontalScaleName = std::string_view("imageHorizontalScale");
    
    unsigned int _kernelSize;
    unsigned int _prewittVerticalTexture, _prewittHorizontalTexture;
    VertexDataBase _vertexData;
    ShaderProgram _shaderProgram;
public:
    PrewittFilter() {
        auto indicesVector = std::vector<unsigned int>(indices, indices + 6);
        _vertexData = VertexData<Layout::Sequential, Vec3>(indicesVector, 4, reinterpret_cast<const float*>(quadCoords));
        _kernelSize = 3;

        const auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "PrewittFilter.vert.glsl");
	    const auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "PrewittFilter.frag.glsl");

        _shaderProgram = ShaderProgram(
            Shader<ShaderType::Vertex>(vertexShaderCode.c_str()),
            Shader<ShaderType::Fragment>(fragmentShaderCode.c_str())
        );
        _shaderProgram.use();
        _shaderProgram.set(kernelSizeName, _kernelSize);

        glGenTextures(1, &_prewittVerticalTexture);
        glBindTexture(GL_TEXTURE_2D, _prewittVerticalTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, _kernelSize, _kernelSize, 0, GL_RED, GL_FLOAT, prewitt_vertical);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &_prewittHorizontalTexture);
        glBindTexture(GL_TEXTURE_2D, _prewittHorizontalTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, _kernelSize, _kernelSize, 0, GL_RED, GL_FLOAT, prewitt_horizontal);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw(unsigned int texture) {
        _shaderProgram.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        _shaderProgram.set(processedTextureName, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _prewittVerticalTexture);
        _shaderProgram.set(prewittVerticalTextureName, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _prewittHorizontalTexture);
        _shaderProgram.set(prewittHorizontalTextureName, 2);

        glActiveTexture(GL_TEXTURE0);
        auto vertexBinding = VertexDataBase::ScopedBinding(_vertexData);
        glDrawElements(GL_TRIANGLES, _vertexData.vertexCount(), GL_UNSIGNED_INT, 0);
    }

    void setMatrixDensity(int width, int height) {
        _shaderProgram.use();
        _shaderProgram.set(kernelVerticalScaleName, static_cast<float>(height));
        _shaderProgram.set(kernelHorizontalScaleName, static_cast<float>(width));
    }

    constexpr static inline glm::vec3 quadCoords[4] = {
        {-1.f, -1.f, 0.f},
        {1.f, -1.f, 0.f},
        {1.f, 1.f, 0.f},
        {-1.f, 1.f, 0.f}
    };

    constexpr static inline unsigned int indices[6] = {
        0, 1, 2, 2, 0, 3
    };

    constexpr static inline float prewitt_vertical[9] = {
        1.0f, 0.0f, -1.f,
        1.0f, 0.0f, -1.f,
        1.0f, 0.0f, -1.f
    };

    constexpr static inline float prewitt_horizontal[9] = {
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,
        -1.f, -1.f, -1.f
    };
};