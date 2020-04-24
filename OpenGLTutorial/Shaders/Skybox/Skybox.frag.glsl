#version 330

out vec4 FragColor;
in vec3 TexCoords; // direction vector representing a 3D texture coordinate
uniform samplerCube cubemap; // cubemap texture sampler

void main()
{             
    FragColor = texture(cubemap, TexCoords);
}  