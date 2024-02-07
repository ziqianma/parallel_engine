#include "textures.h"
#include "timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureData::~TextureData() {
	stbi_image_free(data);
}

void TextureLoader::DeleteTexture(unsigned int shaderProgramID, const std::string& path) {
	if (s_LoadedTextures.count(path) == 1) {
		// get texture unit and set that unit back to being available for its shader
		Texture texture = s_LoadedTextures.find(path)->second;

		s_AvailableTextureUnits[shaderProgramID][texture.texUnit] = 1;

		// deallocate texture from GPU
		unsigned int texID = texture.id;
		glDeleteTextures(1, &texID);

		// delete from loaded textures map, free up texture unit for use
		s_LoadedTextures.erase(path);
	}
}

void TextureLoader::LoadData(const std::string& path, const std::string& type, int id) 
{
	int width = 0, height = 0, nrComponents = 0;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	std::lock_guard<std::mutex> lock(s_ImageMutex);
	s_ProcessingQueue.emplace(id, path, width, height, nrComponents, data, type);
}

void TextureLoader::Update() 
{
	while (!s_ProcessingQueue.empty()) {
		TextureData& toLoad = s_ProcessingQueue.front();

		int nrComponents = toLoad.nrComponents;
		int width = toLoad.width;
		int height = toLoad.height;

		unsigned int id = toLoad.id;

		unsigned char* data = toLoad.data;

		if (data)
		{
			GLenum format = GL_RGB;
			GLenum internalformat = format;
			switch (nrComponents) {
			case 1:
				format = GL_RED;
				internalformat = format;
				break;
			case 3:
				format = GL_RGB;
				internalformat = (toLoad.type == "texture_diffuse") ? GL_SRGB : format;
				break;
			case 4:
				format = GL_RGBA;
				internalformat = (toLoad.type == "texture_diffuse") ? GL_SRGB_ALPHA : format;
				break;
			default:
				break;
			}

			glBindTexture(GL_TEXTURE_2D, id);

			if (width % 4 != 0) {
				// Default row alignment for images is 4
				width = width - (width % 4);
				std::cout << "OpenGL default row alignment for images is 4. Width must be a multiple of 4! Changing to be muliple of 4" << std::endl;
			}

			// Load image data into GL context, delete loaded bitmap data after
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			std::cout << "Loaded: " << toLoad.path << std::endl << "(width, height) : (" << width << ", " << height << "), Texture ID: " << toLoad.id << std::endl;
		}
		else
		{
			std::cout << "Texture failed to load at path: " << toLoad.path << std::endl;
		}
		s_ProcessingQueue.pop();
	}
}

unsigned int TextureLoader::GetAvailableTextureUnit(unsigned int shaderProgramID, const std::string& name)
{
	// initialize available texture units per shader, if they haven't been before
	if (s_AvailableTextureUnits.count(shaderProgramID) == 0) {
		s_AvailableTextureUnits[shaderProgramID].fill(1);
	}

	unsigned int textureUnit = 0;
	for (int i = 0; i < MAX_AVAILABLE_TEXTURE_UNITS; i++) {
		if (s_AvailableTextureUnits[shaderProgramID][i] == 1) {
			std::cout << "Shader(" << shaderProgramID << "): took texture unit(" << i << ") for (" << name << ")" << std::endl;
			textureUnit = i;
			s_AvailableTextureUnits[shaderProgramID][i] = 0;
			break;
		}
	}

	return textureUnit;
}

const Texture& TextureLoader::LoadTexture(unsigned int shaderProgramID, const std::string& path, const std::string &typeName)
{
	if (s_LoadedTextures.count(path) == 1) {
		return s_LoadedTextures.find(path)->second;
	}

	// load texture (async or non-async) then place in loaded texture hashmap
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// If a texture is being initialized for the first time, set that texture unit "unavaialble" and assign
	unsigned int textureUnit = GetAvailableTextureUnit(shaderProgramID, path);

	// push texture obj into loaded texture map.
	s_LoadedTextures.emplace(path, Texture(path, textureID, typeName, textureUnit));

#if ASYNC
	s_Futures.push_back(std::async(std::launch::async, LoadData, path, typeName, textureID));
#else
	TextureFromFile(id, path, false, typeName);
#endif

	return s_LoadedTextures.find(path)->second;
	
}

Texture TextureLoader::LoadSkyboxTexture(unsigned int skyboxShaderID, const std::string& path, const std::string& textureType)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// attach skybox texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (int i = 0; i < game_constants::NUM_SKYBOX_FACES; i++)
	{
		std::string finalPathName = path + "/" + game_constants::SKYBOX_FACES[i];
		unsigned char* data = stbi_load(finalPathName.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << path + "/" + game_constants::SKYBOX_FACES[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	unsigned int textureUnit = GetAvailableTextureUnit(skyboxShaderID, path);

	// move pointer ownership to skybox class
	return Texture(path, textureID, textureType, textureUnit);
}

#if ASYNC
#else
void TextureLoader::TextureFromFile(unsigned int textureID, const std::string& path, bool gamma, const std::string& typeName)
{
	int width, height, nrComponents;
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

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
		std::cout << "Loaded: " << path << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load: " << path << std::endl;
		stbi_image_free(data);
	}
}
#endif