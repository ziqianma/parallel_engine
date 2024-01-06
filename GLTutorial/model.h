#pragma once

#include "mesh.h"
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	Model(const Shader& shader, const std::string& path) : m_Shader(shader) {
		loadModel(path);
	}

	void Draw(const Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	Shader m_Shader;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName);
};