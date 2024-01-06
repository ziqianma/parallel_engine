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
	unsigned int numVerts = mesh->mNumVertices;

	Vertex* vertices = new Vertex[numVerts];
	unsigned int* indices = new unsigned int[numVerts];
	std::vector<Texture> textures;


	// Translate all vertex data
	for (unsigned int i = 0; i < numVerts; i++) {
		
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

		vertices[i] = Vertex(position, normal, texCoord);
	}

	// Translate index data
	int currentIndex = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices[currentIndex + j] = (face.mIndices[j]);
		}
		currentIndex += face.mNumIndices;
	}

	aiColor3D ambient(0.f, 0.f, 0.f);
	aiColor3D diffuse(0.f, 0.f, 0.f);
	aiColor3D specular(0.f, 0.f, 0.f);

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		int diffuseTexCount = material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
		int specularTexCount = material->GetTextureCount(aiTextureType::aiTextureType_SPECULAR);
		int normalTexCount = material->GetTextureCount(aiTextureType::aiTextureType_NORMALS);
		textures.reserve(diffuseTexCount + specularTexCount + normalTexCount);

		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_NORMALS, "texture_bump");

		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	}

	meshes.emplace_back(m_Shader, numVerts, vertices, indices, textures, glm::vec3(ambient.r, ambient.g, ambient.b), glm::vec3(diffuse.r, diffuse.g, diffuse.b), glm::vec3(specular.r, specular.g, specular.b));
}

void Model::loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName) {

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;
		mat->GetTexture(type, i, &path);

		// will put all textures for the current model of type "typeName" into the TextureLoader
		Texture texture = TextureLoader::loadTexture(path.C_Str(), directory, typeName);
		// add previously loaded texture to result list so model contains a reference to all its textures (for drawing to screen)
		textures.push_back(texture);
	}

}
