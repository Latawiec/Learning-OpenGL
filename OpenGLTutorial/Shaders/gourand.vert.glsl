#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Specular;
out vec3 Diffuse;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 norm = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    Diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    Specular = specularStrength * spec * lightColor;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}