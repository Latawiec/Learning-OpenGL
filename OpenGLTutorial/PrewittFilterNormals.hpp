#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "VertexData.hpp"
#include "ShaderProgram.hpp"
#include "Utils.hpp"

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

class PrewittFilterNormals {
    constexpr static auto processedTextureName = std::string_view("imageTexture");
    constexpr static auto kernelVerticalScaleName = std::string_view("imageVerticalScale");
    constexpr static auto kernelHorizontalScaleName = std::string_view("imageHorizontalScale");
    
    VertexDataBase _vertexData;
    ShaderProgram _shaderProgram;
public:
    PrewittFilterNormals() {
        auto indicesVector = std::vector<unsigned int>(indices, indices + 6);
        _vertexData = VertexData<Layout::Sequential, Vec3>(indicesVector, 4, reinterpret_cast<const float*>(quadCoords));

        const auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "PrewittFilter.vert.glsl");
	    const auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "PrewittFilterNormals.frag.glsl");

        _shaderProgram = ShaderProgram(
            Shader<ShaderType::Vertex>(vertexShaderCode.c_str()),
            Shader<ShaderType::Fragment>(fragmentShaderCode.c_str())
        );
    }

    void draw(unsigned int texture) {
        _shaderProgram.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        _shaderProgram.set(processedTextureName, 0);

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