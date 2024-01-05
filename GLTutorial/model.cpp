#include "model.h"

void Model::Draw(const Shader& shader) {
	for (Mesh &m : meshes) {
		m.Draw(shader);
	}
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);		

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/"));
	processNode(scene->mRootNode, scene);
}	

void Model::processNode(aiNode* node, const aiScene* scene) {

	// node->mMeshes contain indicies of meshes inside scene->mMeshes
	// scene->mMeshes contains meshes which make up the entire model.
	// We loop through all references to meshes within the node (indices of meshes inside the scene)
	// Then translate those aiMesh* to our native Mesh type (using processMesh) and add to our mesh list.

	meshes.reserve(scene->mNumMeshes);

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	// Now we recursively process all children of the current node, making sure to add all meshes of the children to our list
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}		
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	//std::vector<Vertex> vertices;

	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);

	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Translate all vertex data
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;

		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		
		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);
		if (mesh->mTextureCoords[0]) {
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.emplace_back(position, normal, texCoord);
	}

	// Translate index data
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	aiColor3D ambient(0.f, 0.f, 0.f);
	aiColor3D diffuse(0.f, 0.f, 0.f);
	aiColor3D specular(0.f, 0.f, 0.f);

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaterials = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaterials.begin(), diffuseMaterials.end());

		std::vector<Texture> specularMaterials = loadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaterials.begin(), specularMaterials.end());

		std::vector<Texture> bumpMaps = loadMaterialTextures(material, aiTextureType::aiTextureType_NORMALS, "texture_bump");
		textures.insert(textures.end(), bumpMaps.begin(), bumpMaps.end());

		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	}

	meshes.emplace_back(vertices, indices, textures, glm::vec3(ambient.r, ambient.g, ambient.b), glm::vec3(diffuse.r, diffuse.g, diffuse.b), glm::vec3(specular.r, specular.g, specular.b));
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> result;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;
		mat->GetTexture(type, i, &path);

		// will put all textures for the current model of type "typeName" into the TextureLoader
		Texture texture = TextureLoader::loadTexture(path.C_Str(), directory, typeName);
		// add previously loaded texture to result list so model contains a reference to all its textures (for drawing to screen)
		result.push_back(texture);
	}

	return result;
}
