#include "model.h"

Model::Model(const std::string& path, const Shader& shader, const std::vector<glm::mat4>& modelMatrices, unsigned int depthTextureID) :
	m_Shader(shader),
	m_Directory(path.substr(0, path.find_last_of("/"))),
	m_NumInstances(modelMatrices.size()),
	m_DepthMapTextureID(depthTextureID),
	m_DepthMapTextureUnit(TextureLoader::GetAvailableTextureUnit(shader.get_shader_id(), "shadow_map"))
{
	shader.bind();
	shader.addUniform1i("shadow_map", m_DepthMapTextureUnit);
	shader.unbind();

	loadModel(path);
	loadInstanceData(modelMatrices);
}

Model::Model(const std::string& path, const Shader& shader, const std::vector<glm::mat4>& modelMatrices) : 
	m_Shader(shader),
	m_Directory(path.substr(0, path.find_last_of("/"))),
	m_NumInstances(modelMatrices.size()),
	m_DepthMapTextureID(0),
	m_DepthMapTextureUnit(0)
{
	loadModel(path);
	loadInstanceData(modelMatrices);
}

void Model::loadInstanceData(const std::vector<glm::mat4>& modelMatrices) 
{
	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (const Mesh& m : m_Meshes) {
		// Bind vertex attributes to VAO
		m.BindMeshVAO();

		// per mesh instacing vertex attribs
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		// Unbind Mesh VAO
		glBindVertexArray(0);
	}
}

void Model::loadModel(const std::string& path) 
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);		

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	processNode(scene->mRootNode, scene);
}	

void Model::processNode(aiNode* node, const aiScene* scene) 
{

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

void Model::processMesh(aiMesh* mesh, const aiScene* scene) 
{

	//Vertex* vertices = new Vertex[numVerts];
	//unsigned int* indices = new unsigned int[numVerts];

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	UnloadedTextureList textures;

	unsigned int numVerts = mesh->mNumVertices;
	vertices.reserve(numVerts);
	indices.reserve(numVerts);

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

		vertices.emplace_back(position, normal, texCoord);
	}

	// Translate index data
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		int diffuseTexCount = material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
		int specularTexCount = material->GetTextureCount(aiTextureType::aiTextureType_SPECULAR);
		int normalTexCount = material->GetTextureCount(aiTextureType::aiTextureType_NORMALS);
		textures.reserve(diffuseTexCount + specularTexCount + normalTexCount);

		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
		loadMaterialTextures(textures, material, aiTextureType::aiTextureType_HEIGHT, "texture_bump");
	}

	m_Meshes.emplace_back(Mesh(m_Shader, vertices, indices, textures, numVerts, m_NumInstances, m_DepthMapTextureID, m_DepthMapTextureUnit));
}

void Model::loadMaterialTextures(UnloadedTextureList& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName) 
{
	aiColor3D ambient, diffuse, specular;
	mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);

	m_Shader.bind();
	m_Shader.addUniform3f("material.ambient", ambient.r, ambient.g, ambient.b);
	m_Shader.addUniform3f("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
	m_Shader.addUniform3f("material.specular", specular.r, specular.g, specular.b);
	m_Shader.addUniform1f("material.shininess", 128.0f);
	m_Shader.unbind();
	
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString texturePath;
		mat->GetTexture(type, i, &texturePath);

		// add to textures list to pass to mesh
		textures.emplace_back(std::make_pair(m_Directory + "/" + std::string(texturePath.C_Str()), typeName));
	}
}

void Model::Draw(const Shader& shader) {
	for (Mesh& mesh : m_Meshes) {
		mesh.Draw(shader);
	}
}
