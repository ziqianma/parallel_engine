#pragma once
#include "common.h"

struct Texture {
	unsigned int id;
	std::string type; //"diffuse" or "specular"
	const char* path;
};

class TextureLoader {
public:
	static Texture loadTexture(const char* path, const std::string& directory, std::string typeName);
private:
	static std::map<const std::string, Texture> loadedTextures;
	static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
};