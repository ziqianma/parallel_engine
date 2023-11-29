#pragma once

#include "common.h"
#include "shader.h"
#include "textures.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(0.7f), glm::vec3 specular = glm::vec3(1.0f));
	void Draw(Shader& shader);
private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};