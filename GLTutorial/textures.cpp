#include "textures.h"
#include "timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::unordered_map<std::string, Texture> TextureLoader::loadedTextures;

Texture TextureLoader::loadTexture(const char* path, const std::string& directory, std::string typeName) {
	AutoProfiler profiler("TextureLoader::loadTexture");
	for (const auto& [texPath, texture] : loadedTextures) {
		if (path == texPath) {
			return texture;
		}
	}

	int id = TextureFromFile(path, directory, false);

	std::string textureKey = std::string(path);
	loadedTextures[textureKey] = Texture(id, typeName);
	
	return Texture(id, typeName);
}

unsigned char* LoadData(const char* filename, int* width, int* height, int* nrComponents) {
	AutoProfiler profiler("TextureLoader::LoadData");
	return stbi_load(filename, width, height, nrComponents, 0);
}

unsigned int TextureLoader::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	AutoProfiler profiler("TextureLoader::TextureFromFile");
	std::cout << "Loading: " << path << std::endl;
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = LoadData(filename.c_str(), &width, &height, &nrComponents);
	//data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
