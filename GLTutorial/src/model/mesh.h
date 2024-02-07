#pragma once

#include "common.h"
#include "shader/shader.h"
#include "util/textures.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	Vertex() : position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), texCoord(glm::vec2(0.0f)) {}
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord) : position(position), normal(normal), texCoord(texCoord) {}
};

class Mesh {
public:
	Mesh(const Shader& shader, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const UnloadedTextureList& textures, unsigned int numVerts, unsigned int numInstances, unsigned int depthTextureID, unsigned int depthTextureUnit);
	void Draw(const Shader& shader);
	void BindMeshVAO() const { glBindVertexArray(VAO); };
private:
	unsigned int VAO, VBO, EBO;
	std::vector<Texture> m_Textures;

	unsigned int m_DepthTextureID;
	unsigned int m_DepthTextureUnit;

	unsigned int m_NumVerts;
	unsigned int m_NumInstances;

	void setup_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	void load_textures(const Shader& shader, const UnloadedTextureList& textures);
};
