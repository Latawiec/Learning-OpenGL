#version 330 core

const int MAX_TEXTURES = 4;

struct Material {
    sampler2D diffuseTextures[MAX_TEXTURES];
    sampler2D specularTextures[MAX_TEXTURES];
    float shininess;
};

struct DumbPointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcAmbient(DumbPointLight light, sampler2D diffuseTex, vec2 texCoords) {
    return light.ambient * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcDiffuse(DumbPointLight light, sampler2D diffuseTex, vec2 texCoords, vec3 normal, vec3 fragPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    return light.diffuse * diff * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcSpecular(DumbPointLight light, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return light.specular * spec * vec3(texture(specularTex, texCoords));
}

vec3 CalcLight(DumbPointLight light, sampler2D diffuseTex, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    return CalcAmbient(light, diffuseTex, texCoords) +
           CalcDiffuse(light, diffuseTex, texCoords, normal, fragPos) +
           CalcSpecular(light, specularTex, shininess, texCoords, normal, fragPos, viewPos);
}

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcAmbient(DirectionalLight light, sampler2D diffuseTex, vec2 texCoords) {
    return light.ambient * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcDiffuse(DirectionalLight light, sampler2D diffuseTex, vec2 texCoords, vec3 normal) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    return light.diffuse * diff * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcSpecular(DirectionalLight light, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return light.specular * spec * vec3(texture(specularTex, texCoords));
}

vec3 CalcLight(DirectionalLight light, sampler2D diffuseTex, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    return CalcAmbient(light, diffuseTex, texCoords) +
           CalcDiffuse(light, diffuseTex, texCoords, normal) +
           CalcSpecular(light, specularTex, shininess, texCoords, normal, fragPos, viewPos);
}

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

float CalcAttuneation(PointLight light, vec3 fragPos) {
    float dist = length(light.position - fragPos);
    return 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
}

vec3 CalcAmbient(PointLight light, sampler2D diffuseTex, vec2 texCoords, vec3 fragPos) {
    return CalcAttuneation(light, fragPos) * light.ambient * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcDiffuse(PointLight light, sampler2D diffuseTex, vec2 texCoords, vec3 normal, vec3 fragPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    return CalcAttuneation(light, fragPos) * light.diffuse * diff * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcSpecular(PointLight light, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return CalcAttuneation(light, fragPos) * light.specular * spec * vec3(texture(specularTex, texCoords));
}

vec3 CalcLight(PointLight light, sampler2D diffuseTex, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    return CalcAmbient(light, diffuseTex, texCoords, fragPos) +
           CalcDiffuse(light, diffuseTex, texCoords, normal, fragPos) +
           CalcSpecular(light, specularTex, shininess, texCoords, normal, fragPos, viewPos);
}

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoffStart;
    float cutoffEnd;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

float CalcCutoff(SpotLight light, vec3 fragPos) {
    float theta     = dot(normalize(fragPos - light.position), normalize(light.direction));
    float epsilon   = light.cutoffStart - light.cutoffEnd;
    return clamp((theta - light.cutoffEnd) / epsilon, 0.0, 1.0);  
}

float CalcAttuneation(SpotLight light, vec3 fragPos) {
    float dist = length(light.position - fragPos);
    return 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
}

vec3 CalcAmbient(SpotLight light, sampler2D diffuseTex, vec2 texCoords, vec3 fragPos) {
    return CalcAttuneation(light, fragPos) * light.ambient * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcDiffuse(SpotLight light, sampler2D diffuseTex, vec2 texCoords, vec3 normal, vec3 fragPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    return CalcCutoff(light, fragPos) * CalcAttuneation(light, fragPos) * light.diffuse * diff * vec3(texture(diffuseTex, texCoords));
}

vec3 CalcSpecular(SpotLight light, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return CalcCutoff(light, fragPos) * CalcAttuneation(light, fragPos) * light.specular * spec * vec3(texture(specularTex, texCoords));
}

vec3 CalcLight(SpotLight light, sampler2D diffuseTex, sampler2D specularTex, float shininess, vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewPos) {
    return CalcAmbient(light, diffuseTex, texCoords, fragPos) +
           CalcDiffuse(light, diffuseTex, texCoords, normal, fragPos) +
           CalcSpecular(light, specularTex, shininess, texCoords, normal, fragPos, viewPos);
}

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform SpotLight spotLight;
uniform PointLight pointLight;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gAlbedo;
layout (location = 2) out vec3 gNormal;

void main()
{
    vec3 result = CalcLight(spotLight, material.diffuseTextures[0], material.specularTextures[0], material.shininess, TexCoords, Normal, FragPos, viewPos)
     + CalcLight(pointLight, material.diffuseTextures[0], material.specularTextures[0], material.shininess, TexCoords, Normal, FragPos, viewPos);
    gAlbedo = vec4(result, 1.0);
    gPosition = FragPos;
    gNormal = normalize(Normal);
}
