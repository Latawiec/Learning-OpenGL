#pragma once 

#include <tuple>
#include <vector>
#include <stdint.h>
#include <glad/glad.h>
#include <cstddef>


enum class Layout : uint8_t {
    Sequential,
    Interleaving
};

// This is pretty straight forward. vec3 -> VertexAttribute<3, float>... right?
template<int Count, typename T>
struct VertexAttribute {
    using value_type = std::decay_t<T>;
    using elements_count = std::integral_constant<int, Count>;
    using byteSize = std::integral_constant<int, Count * sizeof(value_type)>;
    constexpr static int glType() {
        if constexpr(std::is_same_v<value_type, float>) return GL_FLOAT;
        return -1;
    }
    static_assert(glType() != -1, "Given type is not supported or does not have a valid GL type assigned.");
};

using Vec3 = VertexAttribute<3, float>;
using Vec2 = VertexAttribute<2, float>;
using Float = VertexAttribute<1, float>;
// More?

class VertexDataBase {   
protected: 
    unsigned int _VAO{}, _VBO{}, _EBO{};
    size_t _size;
public:
    constexpr VertexDataBase(const std::size_t size) :
    _size(_size) {}

    constexpr void setIndices(const std::vector<unsigned int>& indices) {
        glGenBuffers(1, &_EBO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};

template <Layout, class ... >
class VertexData {};


template <class ... VertexAttributeDescription>
class VertexData<Layout::Interleaving, VertexAttributeDescription...> : public VertexDataBase {
public:    
    constexpr VertexData(const size_t size, const std::byte* data)
    : VertexDataBase(size)
    {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        layoutInterleavingData(data);
        layoutInterleavingAttributes<VertexAttributeDescription...>();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    inline void layoutInterleavingData(const std::byte* data) {
        const int bufferByteSize = _size * (VertexAttributeDescription::byteSize::value + ...);
        glBufferData(GL_ARRAY_BUFFER, dataByteSize, attrDataTuple.data(), GL_STATIC_DRAW);
    }

    template<class LastOfDescriptions>
    inline void layoutInterleavingAttributes(const int index = 0, const int offset = 0) {
        const int stride = (VertexAttributeDescription::byteSize::value + ...);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, HeadOfDescriptions::elements_count::value, HeadOfDescriptions::glType(), GL_FALSE, stride, offset);
    }

    template<class HeadOfDescriptions, class ... RestOfDescriptions>
    inline void layoutInterleavingAttributes(const int index = 0, const int offset = 0) {
        const int stride = (VertexAttributeDescription::byteSize::value + ...);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, HeadOfDescriptions::elements_count::value, HeadOfDescriptions::glType(), GL_FALSE, stride, offset);
        layoutSequentialAttributes<RestOfDescriptions...>(index + 1, offset + HeadOfDescriptions::byteSize::value);
    }
};

template <class ... VertexAttributeDescription>
class VertexData<Layout::Sequential, VertexAttributeDescription...> : public VertexDataBase {
public:    
    constexpr VertexData(const size_t size, const VertexAttributeDescription::value_type* ... data)
    : VertexDataBase(size)
    {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        layoutSequentialData(data...);
        layoutSequentialAttributes<VertexAttributeDescription...>();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    constexpr VertexData(const std::vector<unsigned int> indices, const size_t size, const VertexAttributeDescription::value_type* ... data)
    : VertexDataBase(size)
    {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        layoutSequentialData(data...);
        layoutSequentialAttributes<VertexAttributeDescription...>();
        

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    template<int dataOffset = 0, class LastOfDescriptions>
    inline void layoutSequentialData(const LastOfDescriptions::value_type* data) {
        glBufferSubData(GL_ARRAY_BUFFER, dataOffset * _size, LastOfDescriptions::byteSize * _size, data);
    }

    template<int dataOffset = 0, class HeadOfDescriptions, class ... RestOfDescriptions>
    inline void layoutSequentialData(const HeadOfDescriptions::value_type* data, RestOfDescriptions::value_type* ... rest) {
        glBufferSubData(GL_ARRAY_BUFFER, dataOffset * _size, HeadOfDescriptions::byteSize * _size, data);
        layoutSequentialData<HeadOfDescriptions::byteSize>(rest...);
    }

    template<class LastOfDescriptions>
    inline void layoutSequentialAttributes(const int index = 0, const int offset = 0) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, LastOfDescriptions::elements_count::value, LastOfDescriptions::glType(), GL_FALSE, LastOfDescriptions::byteSize::value, offset);
    }

    template<class HeadOfDescriptions, class ... RestOfDescriptions>
    inline void layoutSequentialAttributes(const int index = 0, const int offset = 0) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, HeadOfDescriptions::elements_count::value, HeadOfDescriptions::glType(), GL_FALSE, HeadOfDescriptions::byteSize::value, offset);
        layoutSequentialAttributes<RestOfDescriptions...>(index + 1, HeadOfDescriptions::byteSize::value * _size);
    }
};