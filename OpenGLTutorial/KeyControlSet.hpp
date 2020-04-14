#pragma once

#include <glfw/glfw3.h>

#include <compare>
#include <map>
#include <set>
#include <functional>
#include <type_traits>
#include <algorithm>

struct Key {
	using KeyIdType = int;

	Key(KeyIdType id) : _id(id) {}

	KeyIdType id() const { return _id; }
	operator KeyIdType() const { return _id; }
	auto operator<=>(const Key& other) const = default;
private:
	KeyIdType _id;
};

enum class State : bool {
	UP = false,
	DOWN = true
};

class KeyControlSet {
	using Callback = std::function<void(Key, State)>;
	std::map<Key, State> _keyStates;
	std::multimap<Key::KeyIdType, Callback> _keyBoundFunctions;
	GLFWwindow* _window;
public:
	KeyControlSet(GLFWwindow* window) : _window(window) {}

	// Basically I need a pair for variadic input ... xD
	template<class ... Pairs, typename enabled = std::enable_if_t<std::conjunction_v<std::is_convertible<Pairs, std::pair<Key::KeyIdType, Callback>>...>>>
	KeyControlSet(GLFWwindow* window, Pairs&& ... keysToFunctions)
	: KeyControlSet(window) {
		(addControl(std::move(keysToFunctions.first), std::move(keysToFunctions.second)), ...);
	}

	void addControl(Key::KeyIdType keyId, std::function<void(Key, State)> function) {
		if (const auto existingRecord = _keyStates.find(keyId); existingRecord != _keyStates.end()) {
			auto& [key, state] = *existingRecord;
			// First-time notification.
			function(key, state);
			_keyBoundFunctions.emplace(keyId, std::move(function));
		} else {
			// Read the state as we don't know anything about the key yet.
			auto key = Key(keyId);
			auto state = static_cast<State>(glfwGetKey(_window, keyId) == GLFW_PRESS);
			// First-time notification.
			function(key, state);
			_keyStates.emplace(key, state);
			_keyBoundFunctions.emplace(keyId, std::move(function));
		}
	}

	void processInput() {
		for (auto& [key, state] : _keyStates) {
			if (auto currentState = static_cast<State>(glfwGetKey(_window, key.id()) == GLFW_PRESS); state != currentState) {
				state = currentState;
				auto [keyCallbacksBegin, keyCallbacksEnd] = _keyBoundFunctions.equal_range(key.id());
				std::for_each(keyCallbacksBegin, keyCallbacksEnd, [&key, &state](auto& keyCallbackPair) {
					auto& [keyId, callback] = keyCallbackPair;
					callback(key, state);
				});
			}
		}
	}
};