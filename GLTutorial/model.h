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
	Model(const std::string& path, const Shader& shader);
	void Draw(const Shader& shader);
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	Shader m_Shader;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName);
};