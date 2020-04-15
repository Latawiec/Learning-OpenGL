#pragma once

#include <GLFW/glfw3.h>

class State {
	enum class KeyState : bool {
		Pressed = true,
		Released = false
	};
	KeyState _state = KeyState::Released;
public:
	State(bool isPressed = false) : _state(static_cast<KeyState>(isPressed)) {}
	bool isPressed() { return _state == KeyState::Pressed; }
};

class KeyControlSet {
	GLFWwindow* _window;
	using Key = int;
public:
	KeyControlSet(GLFWwindow* window) : _window(window) {}

	State keyState(Key key) const {
		return State(glfwGetKey(_window, key) == GLFW_PRESS);
	}
};