#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gAlbedo;
layout (location = 2) out vec3 gNormal;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate
uniform samplerCube cubemap; // cubemap texture sampler

void main()
{             
    gPosition = vec3(0);
    gAlbedo = texture(cubemap, TexCoords);
    gNormal = vec3(0);
}  