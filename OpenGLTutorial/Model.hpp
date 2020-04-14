#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "ShaderProgram.hpp"


class Model {
    std::vector<Mesh> _meshes;
    std::string _directory;
public:

    Model(std::string_view filepath) {
        loadModel(filepath);
    }

    void Draw(ShaderProgram& shader) {
        for (auto& mesh : _meshes) {
            mesh.Draw(shader);
        }
    }
private:
    void loadModel(std::string_view filepath) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(std::string(filepath), aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("Failed to load Model");
        }
        _directory = filepath.substr(0, filepath.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene) {
        for(int i=0; i<node->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }

        for(int i=0; i<node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (int i=0; i<mesh->mNumVertices; ++i) {
            Vertex vertex;

            const auto& position = mesh->mVertices[i];
            vertex.position = glm::vec3(
                position.x, position.y, position.z
            );

            const auto& normal = mesh->mNormals[i];
            vertex.normal = glm::vec3(
                normal.x, normal.y, normal.z
            );

            if (mesh->mTextureCoords[0]) {
                const auto& textureCoords = mesh->mTextureCoords[0][i];
                vertex.texCoord = glm::vec2(textureCoords.x, textureCoords.y);
            } else {
                vertex.texCoord = glm::vec2(0.f);
            }
            
            vertices.push_back(vertex);
        }

        for (int i=0; i<mesh->mNumFaces; ++i) {
            const auto& face = mesh->mFaces[i];
            for (int j=0; j<face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >=0) {
            auto material = scene->mMaterials[mesh->mMaterialIndex];
            auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::Normal);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }

        return Mesh(std::move(vertices), std::move(indices), std::move(textures));
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName) {
        std::vector<Texture> textures;
        for (int i=0; i<mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            textures.emplace_back(_directory + '/' + str.C_Str(), typeName);
        }
        return textures;
    }
};