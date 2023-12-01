#include "mesh.h"

// Constrctor intializes mesh data along with VAO, VBO and EBO
Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	setupMesh();
}

Mesh::Mesh(const std::vector<float> &positions, const std::vector<float> &normals, const std::vector<float> &textureCoords, std::vector<unsigned int>& indices, std::vector<Texture>& textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	this->indices = indices;
	this->textures = textures;

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;

	setupMeshWithSubBuffers(positions, normals, textureCoords);
}

void Mesh::setupMeshWithSubBuffers(const std::vector<float> &positions, const std::vector<float>& normals, const std::vector<float>& textureCoords) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	GLsizeiptr positionsSize = positions.size() * sizeof(float);
	GLsizeiptr normalsSize = normals.size() * sizeof(float);
	GLsizeiptr texCoordsSize = textureCoords.size() * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, positionsSize + normalsSize + texCoordsSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSize, &positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positionsSize, normalsSize, &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positionsSize + normalsSize, texCoordsSize, &textureCoords[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// bind vertex attribs
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)positionsSize);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positionsSize + normalsSize));

	glBindVertexArray(0);

}

void Mesh::setupMesh() {
	// Generate VAO and VBO/EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO/EBO to current VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Bind vertex attributes to VAO

	// First attrib, position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // stride is the size of each VERTEX, attribs are determined through size and offset.

	// Second attrib, normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); //offsetof(S, M) gets the byte offset of variable M in struct S

	// Third attrib, texture coordinate
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		shader.addUniform1i(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	shader.addUniform3f("material.ambient", ambient.r, ambient.g, ambient.b);
	shader.addUniform3f("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
	shader.addUniform3f("material.specular", specular.r, specular.g, specular.b);

	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}