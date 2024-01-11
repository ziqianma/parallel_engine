#pragma once
#include "common.h";
#include <future>
#include <queue>

// Define the maximum available texture units
// In reality, EACH shader takes in 16 textures, but for simplicity this is currently set to 16.
// TODO - maybe a hash map between (shader -> available texture units), when loadTexture is called
#define MAX_AVAILABLE_TEXTURE_UNITS 16

struct Texture {
	unsigned int id;
	unsigned int texUnit;
	std::string type; //"diffuse" or "specular"

	Texture() : id(0), type(""), texUnit(0) {}
	Texture(unsigned int id, const std::string& type, unsigned int texUnit) : id(id), type(type), texUnit(texUnit) {}
};

struct TextureData {
	std::string path;
	Texture texture;
	int width;
	int height;
	int nrComponents;
	unsigned char* data;

	TextureData(const Texture& texture, const std::string& path, int width, int height, int nrComponents, unsigned char* data) : texture(texture), path(path), width(width), height(height), nrComponents(nrComponents), data(data) {}
};

#define ASYNC 1
// TODO - make this a singleton
class TextureLoader {
public:
	static Texture LoadTexture(const std::string& path, const std::string& typeName);

	static void LoadData(const std::string& path, const Texture& texture);
	static void Update();

	static void TextureFromFile(unsigned int textureID, const std::string& path, bool gamma, const std::string& typeName);
	static Texture LoadSkyboxTexture(const std::string &path, const std::vector<std::string>& faceNames);
private:
	static std::vector<std::future<void>> s_Futures;

	static std::unordered_map<std::string, Texture> s_LoadedTextures;
	static std::queue<TextureData> s_ProcessingQueue;
	static std::queue<unsigned int> s_AvailableTextureUnits;
};