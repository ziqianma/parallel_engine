#pragma once
#include "common.h"
#include <future>
#include <stack>

struct Texture {
	unsigned int id;
	std::string type; //"diffuse" or "specular"
	int width;
	int height;
	int nrComponents;
	unsigned char* data;

	Texture() : width(0), height(0), nrComponents(0), data(nullptr) { id = 0; type = ""; }
	Texture(unsigned int id, const std::string& type) : id(id), type(type), width(0), height(0), nrComponents(0), data(nullptr) {}
	Texture(unsigned int id, const std::string& type, int width, int height, int nrComponents, unsigned char* data) : id(id), type(type), width(width), height(height), nrComponents(nrComponents), data(data) {}

};
class TextureLoader {
public:
	static void LoadData(std::string filename, std::string type, unsigned int textureID);
	static Texture loadTexture(const char* path, const std::string& directory, const std::string& typeName);
	static void UpdateTextures();
private:
	static std::vector<std::future<void>> futures;
	static std::stack<Texture> images;
	static std::unordered_map<std::string, Texture> loadedTextures;
	static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma, const std::string& typeName);
};