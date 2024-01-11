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

	Mesh(Vertex* vertices, unsigned int* indices, const std::vector<Texture>& textures, unsigned int numVerts);

	void Draw(const Shader& shader);
	void setupTextures(const Shader& shader);
private:
	unsigned int VAO, VBO, EBO;

	Vertex* m_Vertices;
	unsigned int* m_Indices;
	std::vector<Texture> m_Textures;

	unsigned int m_NumVerts;

	void setupMesh();
};
