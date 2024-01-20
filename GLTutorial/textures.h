#pragma once


// Define the maximum available texture units
// In reality, EACH shader takes in 16 textures, but for simplicity this is currently set to 16.
// TODO - maybe a hash map between (shader -> available texture units), when loadTexture is called
#define MAX_AVAILABLE_TEXTURE_UNITS 16

#include "common.h"
#include <future>
#include <queue>

struct Texture {
	int id;
	unsigned int texUnit;
	std::string type; //"diffuse" or "specular"

	Texture(int id, const std::string& type, unsigned int texUnit) : id(id), type(type), texUnit(texUnit) {}
	~Texture();
};

struct TextureData {
	int id;

	std::string path;
	int width;
	int height;
	int nrComponents;
	unsigned char* data;

	TextureData(int id, const std::string& path, int width, int height, int nrComponents, unsigned char* data) : id(id), path(path), width(width), height(height), nrComponents(nrComponents), data(data) {}
	~TextureData();
};

#define ASYNC 1
class TextureLoader {
public:
	static void LoadTexture(const std::string& path, const std::string& typeName);
	static const Texture& GetTexture(const std::string& path);
	static void DeleteTexture(const std::string& path);

	static void Update();

	static std::unique_ptr<Texture> LoadSkyboxTexture(const std::string &path, const std::vector<std::string>& faceNames, const std::string& textureType);
private:
	static std::vector<std::future<void>> s_Futures;

	static std::unordered_map<std::string, std::unique_ptr<Texture>> s_LoadedTextures;
	static std::queue<TextureData> s_ProcessingQueue;
	static std::queue<unsigned int> s_AvailableTextureUnits;

	static void LoadData(const std::string& path, int textureID);
	static void TextureFromFile(unsigned int textureID, const std::string& path, bool gamma, const std::string& typeName);
};