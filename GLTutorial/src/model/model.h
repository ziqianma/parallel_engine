#pragma once

#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <unordered_set>

/*
* Model.h
* 
* While model refers to an entire "model", that is, a .obj file defining associated meshes and materials,
* It would be more descriptive to refer to it as a "mesh group",
* As it does the processing for each mesh, all the materials and its associated textures, and also uploads the offset (modelMatrices) as a buffer,
* But does not do any of the actual drawing or vertex data setup (which is done in Mesh.h)
 */

class Model {
public:
	Model(const std::string& path, const Shader& shader, const std::vector<glm::mat4>& modelMatrices);
	~Model();
	void Draw(const Shader& shader);
private:
	std::vector<Mesh> m_Meshes;
	std::unordered_set<std::string> m_ModelTexturePaths;
	std::string m_Directory;
	Shader m_Shader;
	unsigned int m_NumInstances;
	unsigned int m_InstanceVBO;

	void loadInstanceData(const std::vector<glm::mat4>& modelMatrices);
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);		
	void loadMaterialTextures(std::vector<std::string>& texturePaths, aiMaterial* mat, aiTextureType type, const std::string& typeName);

};	