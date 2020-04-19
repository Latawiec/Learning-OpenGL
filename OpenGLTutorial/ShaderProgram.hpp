#pragma once
#include <glad/glad.h>
#include <stdint.h>
#include <string_view>

enum class ShaderType : uint8_t {
	Vertex,
	Fragment
};

template<ShaderType Type>
class Shader {
	unsigned int _id;
public:
	Shader (std::string_view shaderCode) {
		_id = glCreateShader(mapShaderTypeToGLType(Type));
		compileShader(shaderCode.data());
	}

	~Shader() {
		glDeleteShader(_id);
	}

	Shader(Shader& other) = delete;
	Shader(Shader&& other) = delete;
	Shader& operator=(Shader& other) = delete;
	Shader& operator=(Shader&& other) = delete;

	unsigned int Id() const {
		return _id;
	}

	operator unsigned int() const {
		return _id;
	}

private:
	void compileShader(const char* shaderCode) {
		glShaderSource(_id, 1, &shaderCode, NULL);
		glCompileShader(_id);

		#ifndef NDEBUG
		int success;
		char infoLog[512];
		glGetShaderiv(_id, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(_id, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		#endif
	}

	int mapShaderTypeToGLType(const ShaderType type) {
		switch(type) {
			case ShaderType::Vertex:
				return GL_VERTEX_SHADER;
			case ShaderType::Fragment:
				return GL_FRAGMENT_SHADER;
			default:
				return -1;
		}
	}
};

class ShaderProgram {
	unsigned int _id = 0;
public:
	template<ShaderType ... Ts>
	ShaderProgram(const Shader<Ts>& ... shaderParts) {
		_id = glCreateProgram();
		(glAttachShader(_id, shaderParts), ...);
		linkProgram();
	}

	ShaderProgram() = default;

	ShaderProgram(ShaderProgram&& other) {
		glDeleteProgram(_id);
		_id = other._id;
		other._id = 0;
	}

	ShaderProgram& operator=(ShaderProgram&& other) {
		glDeleteProgram(_id);
		_id = other._id;
		other._id = 0;
		return *this;
	}

	~ShaderProgram() {
		if (_id != 0) {
			glDeleteProgram(_id);
		}
	}

	unsigned int Id() const {
		return _id;
	}

	operator unsigned int() const {
		return _id;
	}

	void use() const {
		glUseProgram(_id);
	}

	template<class T>
	void set(const std::string_view name, const T value) {
		static_assert("Not implemented for given type.");
	}

private:
	void linkProgram() {
		glLinkProgram(_id);

		#ifndef NDEBUG
		int success;
		char infoLog[512];
		glGetProgramiv(_id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(_id, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';
		}
		#endif
	}
};

template<>
void ShaderProgram::set<bool>(const std::string_view name, const bool value) {
	glUniform1i(glGetUniformLocation(_id, name.data()), (int)value);
}

template<>
void ShaderProgram::set<int>(const std::string_view name, const int value) {
	glUniform1i(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set<unsigned int>(const std::string_view name, const unsigned int value) {
	glUniform1i(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set<float>(const std::string_view name, const float value) {
	glUniform1f(glGetUniformLocation(_id, name.data()), value);
}

template<>
void ShaderProgram::set<glm::mat4>(const std::string_view name, const glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(_id, name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void ShaderProgram::set<glm::vec3>(const std::string_view name, const glm::vec3 value) {
	glUniform3fv(glGetUniformLocation(_id, name.data()), 1, glm::value_ptr(value));
}