#include "textures.h"
#include "timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::vector<std::future<void>> TextureLoader::futures;
std::unordered_map<std::string, Texture> TextureLoader::loadedTextures;
std::stack<TextureData> TextureLoader::imageDataToLoad;

static std::mutex s_imageMutex;

void TextureLoader::LoadData(std::string filename, std::string type, unsigned int textureID) {
	int width = 0, height = 0, nrComponents = 0;
	std::lock_guard<std::mutex> lock(s_imageMutex);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	imageDataToLoad.push(TextureData(textureID, width, height, nrComponents, data));
}

void TextureLoader::Update() {
	while (!imageDataToLoad.empty()) {
		TextureData toLoad = imageDataToLoad.top();
		int nrComponents = toLoad.nrComponents;
		int width = toLoad.width;
		int height = toLoad.height;
		int id = toLoad.id;
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
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			std::cout << "Loaded: " << id << std::endl;
		}
		else
		{
			std::cout << "Texture failed to load: " << id << std::endl;
			stbi_image_free(data);
		}
		imageDataToLoad.pop();
	}
}

Texture TextureLoader::loadTexture(const char* path, const std::string& directory, const std::string &typeName) {
	for (const auto& [texPath, texture] : loadedTextures) {
		if (path == texPath) {
			return texture;
		}
	}	

	int id = TextureFromFile(path, directory, false, typeName);
	return Texture(id, typeName);
}

unsigned int TextureLoader::TextureFromFile(const char* path, const std::string& directory, bool gamma, const std::string& typeName)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);
#define ASYNC 1
#if ASYNC
	futures.push_back(std::async(std::launch::async, LoadData, filename, typeName, textureID));
#else
	data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

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
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		std::cout << "Loaded: " << id << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load: " << id << std::endl;
		stbi_image_free(data);
	}
#endif

	std::string textureKey = std::string(path);
	loadedTextures[textureKey] = Texture(textureID, typeName);

	return textureID;
}
