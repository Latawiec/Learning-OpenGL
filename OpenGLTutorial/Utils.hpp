#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Utils {

static std::string readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}

	const size_t dataSize = file.tellg();
	std::vector<char> result(dataSize);
	file.seekg(0);

	file.read(result.data(), dataSize);

	return std::string(result.begin(), result.end());
}

}