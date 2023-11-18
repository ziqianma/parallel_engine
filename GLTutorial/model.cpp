#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  

void Model::Draw(Shader& shader) {
	for (Mesh m : meshes) {
		m.Draw(shader);
	}
}

void Model::loadModel(std::string path) {
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
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// Now we recursively process all children of the current node, making sure to add all meshes of the children to our list
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}		
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Translate all vertex data
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex v;
		glm::vec3 temp;
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		v.position = temp;
		temp.x = mesh->mNormals[i].x;
		temp.y = mesh->mNormals[i].y;
		temp.z = mesh->mNormals[i].z;
		v.normal = temp;

		glm::vec2 texCoord;
		if (mesh->mTextureCoords[0]) {
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
			v.texCoord = texCoord;
		}
		else {
			v.texCoord = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(v);
	}

	// Translate index data
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
		
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaterials = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaterials.begin(), diffuseMaterials.end());

		std::vector<Texture> specularMaterials = loadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaterials.begin(), specularMaterials.end());

	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> result;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;			
		mat->GetTexture(type, i, &path);
		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path, path.C_Str()) == 0)
			{
				result.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(path.C_Str(), directory);
			texture.type = typeName;
			texture.path = path.C_Str();
			result.push_back(texture);
			loadedTextures.push_back(texture);
		}	
	}
	return result;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}