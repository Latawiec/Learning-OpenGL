#pragma once
#include <glm/glm.hpp>

#include <array>

#include "ShaderProgram.hpp"
#include "Utils.hpp"

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

class Gizmo {

    struct LineElement {
        glm::vec3 lineElementPos;
        glm::vec3 lineElementColor;
    };

    std::array<LineElement, 6> _lines = {
        LineElement{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f) },
        LineElement{ glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f) },

        LineElement{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f) },
        LineElement{ glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f) },

        LineElement{ glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f) },
        LineElement{ glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f) }
    };

    unsigned int VAO, VBO;
    ShaderProgram program;
    glm::mat4 transform = glm::mat4(1.f);

public:
    Gizmo() {
        prepareBuffers();
        prepareProgram();
    }
    
    void setDirection(glm::vec3 direction) {
        transform = glm::lookAt(glm::vec3(0.f), direction, glm::vec3(0.f, 1.f, 0.f));
    }

    void draw() {
        program.use();
        program.set("transform", transform);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, _lines.size());
        glBindVertexArray(0);
    }

private:
    void prepareBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, _lines.size() * sizeof(_lines[0]), _lines.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineElement), (void*)offsetof(LineElement, lineElementPos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineElement), (void*)offsetof(LineElement, lineElementColor));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void prepareProgram() {
        auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "gizmo.vert.glsl");
        auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "gizmo.frag.glsl");

        program = ShaderProgram(
            Shader<ShaderType::Vertex>(vertexShaderCode),
            Shader<ShaderType::Fragment>(fragmentShaderCode)
        );
    }
};