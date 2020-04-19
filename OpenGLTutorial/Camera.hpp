#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

    glm::vec3 cameraPos = glm::vec3(-5.f, 0.f, -1.f);
    glm::vec3 cameraFront = glm::vec3(1.f, 0.f, 0.f);
    const glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

    float _yaw = 0.f;
    float _pitch = 0.f;

    float _fov = 45.f;
    float _aspectRatio = 1.f/2.f;

public:
    void updatePosition(const float front, const float left, const float up) {
        cameraPos += cameraFront * front;
        auto vecRight = glm::normalize(glm::cross(cameraFront, cameraUp));
        cameraPos += vecRight * left;
        cameraPos += cameraUp * up;
    }

    void updateRotation(const float pitchOffset, const float yawOffset) {
        _yaw += yawOffset;
        _pitch += pitchOffset;

        _pitch = _pitch > 89.f ? 89.f : _pitch;
        _pitch = _pitch < -89.f ? -89.f : _pitch;

        cameraFront = glm::normalize(glm::vec3(
            cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
            sin(glm::radians(-_pitch)),
            sin(glm::radians(_yaw)) * cos(glm::radians(_pitch))
        ));
    }

    void updateFov(const float fovOffset) {
        _fov -= fovOffset;
        if(_fov <= 1.0f)
            _fov = 1.0f;
        else if(_fov >= 45.0f)
            _fov = 45.0f;
    }

    void updateAspectRatio(const float newAspectRatio) {
        _aspectRatio = newAspectRatio;   
    }

    glm::mat4 getViewTransform() const {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    glm::mat4 getProjectionTransform() const {
        return glm::perspective(glm::radians(_fov), _aspectRatio, 0.1f, 100.f);
    }

    glm::vec3 getPosition() const {
        return cameraPos;
    }

    glm::vec3 getFront() const { 
        return cameraFront;
    }
};
