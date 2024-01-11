#include "model.h"

Model::Model(const std::string& path, const Shader& shader) : m_Shader(shader) {
	m_Directory = path.substr(0, path.find_last_of("/"));
	loadModel(path);
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);		

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	m_Directory = path.substr(0, path.find_last_of("/"));
	processNode(scene->mRootNode, scene);
}	

void Model::processNode(aiNode* node, const aiScene* scene) {

	// node->mMeshes contain indicies of meshes inside scene->mMeshes
	// scene->mMeshes contains meshes which make up the entire model.
	// We loop through all references to meshes within the node (indices of meshes inside the scene)
	// Then translate those aiMesh* to our native Mesh type (using processMesh) and add to our mesh list.

	m_Meshes.reserve(scene->mNumMeshes);

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
			indices[currentIndex + j] = face.mIndices[j];
		}
		currentIndex += face.mNumIndices;
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		int diffuseTexCount = material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
		int specularTexCount = material->GetTextureCount(aiTextureType::aiTextureType_SPECULAR);
		int normalTexCount = material->GetTextureCount(aiTextureType::aiTextureType_NORMALS);
		textures.reserve(diffuseTexCount + specularTexCount + normalTexCount);

		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_NORMALS, "texture_bump");

	}

	m_Meshes.emplace_back(vertices, indices, textures, numVerts);
}

void Model::loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName) {

	aiColor3D ambient(0.f, 0.f, 0.f);
	aiColor3D diffuse(0.f, 0.f, 0.f);
	aiColor3D specular(0.f, 0.f, 0.f);
	//, glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(0.7f), glm::vec3 specular = glm::vec3(1.0f)
	mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);

	m_Shader.bind();
	m_Shader.addUniform3f("material.ambient", ambient.r, ambient.g, ambient.b);
	m_Shader.addUniform3f("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
	m_Shader.addUniform3f("material.specular", specular.r, specular.g, specular.b);
	m_Shader.unbind();
	
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString texturePath;
		mat->GetTexture(type, i, &texturePath);

		std::string path = m_Directory + "/" + std::string(texturePath.C_Str());

		// Load texture from loader
		Texture texture = TextureLoader::LoadTexture(path, typeName);

		// Add as uniform
		// typenumber is i+1 since 0th texture is texture_(type)1
		std::string typeNumber = std::to_string(i + 1);

		m_Shader.bind();
		m_Shader.addUniform1i(("material." + typeName + typeNumber), texture.texUnit);
		m_Shader.unbind();

		// add to textures list to pass to mesh
		textures.push_back(texture);
	}
}

void Model::Draw(const Shader& shader) {
	for (Mesh& mesh : m_Meshes) {
		mesh.Draw(shader);
	}
}
