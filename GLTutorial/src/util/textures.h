#pragma once


// Define the maximum available texture units
// In reality, EACH shader takes in 16 textures, but for simplicity this is currently set to 16.
// TODO - maybe a hash map between (shader -> available texture units), when loadTexture is called
#define MAX_AVAILABLE_TEXTURE_UNITS 16

#include "common.h"
#include <future>
#include <unordered_set>

typedef std::vector<std::pair<std::string, std::string>> UnloadedTextureList;

struct Texture {
	std::string path;
	int id;
	unsigned int texUnit;
	std::string type; //"diffuse" or "specular"

	Texture() = delete;
	Texture(const std::string& path, int id, const std::string& type, unsigned int texUnit) : path(path), id(id), type(type), texUnit(texUnit) {}
};

struct TextureData {
	int id;

	std::string path;
	std::string type;
	int width;
	int height;
	int nrComponents;
	unsigned char* data;

	TextureData(int id, const std::string& path, int width, int height, int nrComponents, unsigned char* data,  std::string type) : id(id), path(path), width(width), height(height), nrComponents(nrComponents), data(data), type(type) {}
	~TextureData();
};

#define ASYNC 1
class TextureLoader {
public:
	static const Texture& LoadTexture(unsigned int shaderProgramID, const std::string& path, const std::string& typeName);
	static void DeleteTexture(unsigned int shaderProgramID, const std::string& path);
	static unsigned int GetAvailableTextureUnit(unsigned int shaderProgramID, const std::string& name);

	static void Update();

	static Texture LoadSkyboxTexture(unsigned int skyboxShaderID, const std::string &path, const std::string& textureType);
private:
	inline static std::vector<std::future<void>> s_Futures;

	inline static std::unordered_map<std::string, Texture> s_LoadedTextures;
	inline static std::queue<TextureData> s_ProcessingQueue;
	inline static std::unordered_map<unsigned int, std::array<unsigned int, MAX_AVAILABLE_TEXTURE_UNITS>> s_AvailableTextureUnits;
	inline static std::mutex s_ImageMutex;

	static void LoadData(const std::string& path, const std::string& type, int textureID);
	static void TextureFromFile(unsigned int textureID, const std::string& path, bool gamma, const std::string& typeName);
};