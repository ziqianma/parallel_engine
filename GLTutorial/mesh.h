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
	Vertex* m_Vertices;
	unsigned int* m_Indices;
	std::vector<Texture> textures;
	unsigned int m_NumVerts;

	Mesh(const Shader& shader, unsigned int numVerts, Vertex* vertices, unsigned int* indices, const std::vector<Texture>& textures, glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(0.7f), glm::vec3 specular = glm::vec3(1.0f));

	void Draw(const Shader& shader);
	void setupTextures(const Shader& shader);
private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};
