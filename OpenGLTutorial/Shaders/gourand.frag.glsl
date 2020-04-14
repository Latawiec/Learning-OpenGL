#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Specular;
in vec3 Diffuse;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    float ambient = 0.1;

    vec3 result = (ambient + Diffuse + Specular) * vec3(mix(
        texture(texture1, TexCoord),
        texture(texture2, TexCoord),
        0.2
    ));

    FragColor = vec4(result, 1.f);
} 