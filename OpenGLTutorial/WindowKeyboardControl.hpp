#pragma once

#include "KeyControlSet.hpp"

class WindowKeyboardControl {
	const KeyControlSet& _keyControlSet;
	GLFWwindow* _window;
public:
	WindowKeyboardControl(GLFWwindow* window, const KeyControlSet& keyControlSet) : _keyControlSet(keyControlSet), _window(window) {}

	void update() {
		if (_keyControlSet.keyState(GLFW_KEY_ESCAPE).isPressed()) glfwSetWindowShouldClose(_window, true);
	}
};