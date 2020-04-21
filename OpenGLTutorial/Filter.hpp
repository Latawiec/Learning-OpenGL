#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "VertexData.hpp"
#include "ShaderProgram.hpp"
#include "Utils.hpp"

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

class Filter {
    constexpr static auto processedTextureName = std::string_view("imageTexture");
    constexpr static auto kernelTextureName = std::string_view("kernelTexture");
    constexpr static auto kernelSizeName = std::string_view("kernelSize");
    constexpr static auto kernelVerticalScaleName = std::string_view("imageVerticalScale");
    constexpr static auto kernelHorizontalScaleName = std::string_view("imageHorizontalScale");
    
    unsigned int _kernelSize;
    unsigned int _kernelTexture;
    VertexDataBase _vertexData;
    ShaderProgram _shaderProgram;
public:
    Filter(const unsigned int kernelSize, const std::vector<glm::vec3>& kernel) {
        auto indicesVector = std::vector<unsigned int>(indices, indices + 6);
        _vertexData = VertexData<Layout::Sequential, Vec3>(indicesVector, 4, reinterpret_cast<const float*>(quadCoords));
        _kernelSize = kernelSize;

        const auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "Filter.vert.glsl");
	    const auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "Filter.frag.glsl");

        _shaderProgram = ShaderProgram(
            Shader<ShaderType::Vertex>(vertexShaderCode.c_str()),
            Shader<ShaderType::Fragment>(fragmentShaderCode.c_str())
        );
        _shaderProgram.use();
        _shaderProgram.set(kernelSizeName, kernelSize);

        glGenTextures(1, &_kernelTexture);
        glBindTexture(GL_TEXTURE_2D, _kernelTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, kernelSize, kernelSize, 0, GL_RGB, GL_FLOAT, kernel.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw(unsigned int texture) {
        _shaderProgram.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        _shaderProgram.set(processedTextureName, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _kernelTexture);
        _shaderProgram.set(kernelTextureName, 1);

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

};