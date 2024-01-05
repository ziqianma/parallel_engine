#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	Model(const std::string& path) {
		loadModel(path);
	}
	void Draw(const Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#endif