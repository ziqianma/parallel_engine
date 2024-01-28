#include "textures.h"
#include "timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::~Texture() {
	// deallocate texture from gl context
	unsigned int texID = id;
	glDeleteTextures(1, &texID);
}

TextureData::~TextureData() {
	stbi_image_free(data);
}

void TextureLoader::DeleteTexture(const std::string& path) {
	// delete from loaded textures map, free up texture unit for use.

	// move unqiue_ptr from static map to this unique ptr (hold reference)
	std::unique_ptr<Texture> texturePtr = std::move(s_LoadedTextures[path]);

	// delete map ref to now hollow unqiue_ptr
	if (texturePtr) {
		s_LoadedTextures.erase(path);

		// get texture unit and add back onto available unit stack
		s_AvailableTextureUnits[texturePtr->texUnit] = 1;

		// delete unique_ptr
		texturePtr.reset();
	}
}

const Texture& TextureLoader::GetTexture(const std::string& path) {
	return *(s_LoadedTextures[path]).get();
}

void TextureLoader::LoadData(const std::string& path, int id) 
{
	int width = 0, height = 0, nrComponents = 0;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	std::lock_guard<std::mutex> lock(s_ImageMutex);
	s_ProcessingQueue.emplace(id, path, width, height, nrComponents, data);
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
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);

			if (width % 4 != 0) {
				// Default row alignment for images is 4
				width = width - (width % 4);
				std::cout << "OpenGL default row alignment for images is 4. Width must be a multiple of 4! Changing to be muliple of 4" << std::endl;
			}

			// Load image data into GL context, delete loaded bitmap data after
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

void TextureLoader::LoadTexture(const std::string& path, const std::string &typeName)
{
	
	// if the loadedTextures map is empty, we can init the available texture units
	// if its empty due to all textures being cleared, its still ok to reset the list
	if (s_LoadedTextures.empty()) {
		s_AvailableTextureUnits.fill(1);
	}

	// check if currently requested texture has already been "loaded", if so, don't do anything.
	for (const auto& [texPath, texture] : s_LoadedTextures) {
		if (path == texPath) {
			return;
		}
	}	

	// load texture (async or non-async) then place in loaded texture hashmap
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// If a texture is being initialized for the first time, set that texture unit "unavaialble" and assign


	unsigned int textureUnit;
	for (int i = 0; i < MAX_AVAILABLE_TEXTURE_UNITS; i++) {
		if (s_AvailableTextureUnits[i] == 1) {
			textureUnit = i;
			s_AvailableTextureUnits[i] = 0;
			break;
		}
	}

	// push uniquely managed texture pointer into loaded texture map.
	s_LoadedTextures[path] = std::make_unique<Texture>(textureID, typeName, textureUnit);

#if ASYNC
	s_Futures.push_back(std::async(std::launch::async, LoadData, path, textureID));
#else
	TextureFromFile(id, path, false, typeName);
#endif
}

std::unique_ptr<Texture> TextureLoader::LoadSkyboxTexture(const std::string& path, const std::string& textureType)
{
	if (s_LoadedTextures.empty()) {
		s_AvailableTextureUnits.fill(1);
	}

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

	unsigned int textureUnit;
	for (int i = 0; i < MAX_AVAILABLE_TEXTURE_UNITS; i++) {
		if (s_AvailableTextureUnits[i] == 1) {
			textureUnit = i;
			s_AvailableTextureUnits[i] = 0;
			break;
		}
	}

	// move pointer ownership to skybox class
	return std::make_unique<Texture>(textureID, textureType, textureUnit);
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