#pragma once
#include "common.h"
#include <future>
#include <stack>

struct TextureData {
	unsigned int id;	
	int width;
	int height;
	int nrComponents;
	unsigned char* data;
	bool loaded = false;

	TextureData(unsigned int id, int width, int height, int nrComponents, unsigned char* data) : id(id), width(width), height(height), nrComponents(nrComponents), data(data) {}
};

struct Texture {
	unsigned int id;
	std::string type; //"diffuse" or "specular"

	Texture() : id(0), type("") {}
	Texture(unsigned int id, const std::string& type) : id(id), type(type) {}
};
class TextureLoader {
public:
	static void LoadData(std::string filename, std::string type, unsigned int textureID);
	static Texture loadTexture(const char* path, const std::string& directory, const std::string& typeName);
	static void Update();
private:
	static std::unordered_map<std::string, Texture> loadedTextures;
	static std::stack<TextureData> imageDataToLoad;
	static std::vector<std::future<void>> futures;
	static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma, const std::string& typeName);
};