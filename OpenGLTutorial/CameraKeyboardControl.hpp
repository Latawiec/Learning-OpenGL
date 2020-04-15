#pragma once

#include "KeyControlSet.hpp"
#include "Camera.hpp"

class CameraKeyboardControl {
	Camera& _camera;
	const KeyControlSet& _keyControlSet;
	float _movementSpeed;
public:
	CameraKeyboardControl(Camera& camera, const KeyControlSet& keyControlSet, float movementSpeed = 0.5f)
	: _camera(camera),
	  _keyControlSet(keyControlSet),
	  _movementSpeed(movementSpeed) {}
	CameraKeyboardControl(const CameraKeyboardControl& other) = delete;
	CameraKeyboardControl* operator=(const CameraKeyboardControl& other) = delete;

	void update(const float elapsedTime) {
		glm::vec3 _update = {};
		if (_keyControlSet.keyState(GLFW_KEY_W).isPressed()) 			_update.x += 1.f;
		if (_keyControlSet.keyState(GLFW_KEY_S).isPressed()) 			_update.x -= 1.f;
		if (_keyControlSet.keyState(GLFW_KEY_D).isPressed()) 			_update.y += 1.f;
		if (_keyControlSet.keyState(GLFW_KEY_A).isPressed()) 			_update.y -= 1.f;
		if (_keyControlSet.keyState(GLFW_KEY_SPACE).isPressed()) 		_update.z += 1.f;
		if (_keyControlSet.keyState(GLFW_KEY_LEFT_SHIFT).isPressed()) 	_update.z -= 1.f;
		_update *= _movementSpeed * elapsedTime;
		_camera.updatePosition(_update.x, _update.y, _update.z);
	}
};