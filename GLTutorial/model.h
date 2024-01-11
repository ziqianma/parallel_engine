#pragma once

#include "mesh.h"
#include <mutex>
#include <future>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	Model(const Shader& shader, const std::string& path) {
		loadModel(path, shader);
	}

	void Draw(const Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::vector<std::future<void>> m_Futures;
	std::string directory;

	void loadModel(const std::string& path, const Shader &shader);
	void processNode(aiNode* node, const aiScene* scene, const Shader &shader);
	void processMesh(aiMesh* mesh, const aiScene* scene, const Shader &shader);
	void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName);
};