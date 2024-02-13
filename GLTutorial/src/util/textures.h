#pragma once


// Define the maximum available texture units
// In reality, EACH shader takes in 16 textures, but for simplicity this is currently set to 16.
// TODO - maybe a hash map between (shader -> available texture units), when loadTexture is called
#define MAX_AVAILABLE_TEXTURE_UNITS 16

#include "common.h"
#include <future>
#include <unordered_set>

typedef std::vector<std::pair<std::string, unsigned int>> UnloadedTextureList;

enum TextureType {
	TEXTURE_AMBIENT = 0,
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_DEPTH,
	TEXTURE_CUBEMAP
};

const std::string type_name_map[5] = {
	game_constants::AMBIENT_TEXTURE_NAME,
	game_constants::DIFFUSE_TEXTURE_NAME,
	game_constants::SPECULAR_TEXTURE_NAME,
	game_constants::DEPTH_TEXTURE_NAME,
	game_constants::CUBEMAP_TEXTURE_NAME
};

struct Texture {
	std::string path;
	unsigned int id;
	unsigned int texUnit;
	TextureType textureType;

	Texture() = delete;
	Texture(const std::string& path, int id, TextureType textureType, unsigned int texUnit) : path(path), id(id), textureType(textureType), texUnit(texUnit) {}
};

struct TextureData {
	Texture texture;

	int width;
	int height;
	int nrComponents;
	unsigned char* data;

	TextureData(Texture texture, int width, int height, int nrComponents, unsigned char* data)
		: texture(texture), width(width), height(height), nrComponents(nrComponents), data(data) {}

	~TextureData();
};

#define ASYNC 1
class TextureLoader {
public:
	static const Texture& LoadTexture(unsigned int shaderProgramID, const std::string& path, TextureType type);
	static void DeleteTexture(unsigned int shaderProgramID, const std::string& path);
	static unsigned int GetAvailableTextureUnit(unsigned int shaderProgramID, const std::string& name);

	static void Update();

	static Texture LoadSkyboxTexture(unsigned int skyboxShaderID, const std::string &path);
private:
	inline static std::vector<std::future<void>> s_Futures;

	inline static std::unordered_map<std::string, Texture> s_LoadedTextures;
	inline static std::queue<TextureData> s_ProcessingQueue;
	inline static std::unordered_map<unsigned int, std::array<unsigned int, MAX_AVAILABLE_TEXTURE_UNITS>> s_AvailableTextureUnits;
	inline static std::mutex s_ImageMutex;

	static void LoadData(Texture texture);
	static void TextureFromFile(unsigned int textureID, const std::string& path, bool gamma, const std::string& typeName);
};