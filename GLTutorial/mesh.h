#pragma once

#include "common.h"
#include "shader.h"
#include "textures.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	Vertex() : position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), texCoord(glm::vec2(0.0f)) {}
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord) : position(position), normal(normal), texCoord(texCoord) {}
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::string>& texturePaths, unsigned int numVerts, unsigned int numInstances);
	~Mesh();
	void Draw(const Shader& shader);
	void BindMeshVAO() { glBindVertexArray(VAO); };
private:
	unsigned int VAO, VBO, EBO;
	std::vector<int> m_TextureIDs;
	std::vector<std::string> m_TexturePaths;
	std::vector<int> m_TextureUnits;

	unsigned int m_NumVerts;
	unsigned int m_NumInstances;

	void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
