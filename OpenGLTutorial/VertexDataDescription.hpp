#pragma once 

#include <stdint.h>


enum class Layout : uint8_t {
    Sequential,
    Interleaving
};


// This is pretty straight forward. vec3 -> VertexAttribute<float, 3>... right?
template<typename T, int Count>
struct VertexAttribute {

};

template<Layout layoutType>
struct VertexData {

    VertexDataDescription()
};