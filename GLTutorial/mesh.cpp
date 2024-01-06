#include "mesh.h"

// Constrctor intializes mesh data along with VAO, VBO and EBO

Mesh::Mesh(const Shader& shader, unsigned int numVerts, Vertex* vertices, unsigned int* indices, const std::vector<Texture>& textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	m_Vertices = vertices;
	m_Indices = indices;
	m_NumVerts = numVerts;
	this->textures = textures;

	shader.addUniform3f("material.ambient", ambient.r, ambient.g, ambient.b);
	shader.addUniform3f("material.diffuse", diffuse.r, diffuse.g, diffuse.b);
	shader.addUniform3f("material.specular", specular.r, specular.g, specular.b);
	setupTextures(shader);
	setupMesh();

	delete[] m_Vertices;
	delete[] m_Indices;
}


void Mesh::setupMesh() {
	// Generate VAO and VBO/EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind vertex attributes to VAO
	glBindVertexArray(VAO);

	// Bind VBO/EBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_NumVerts * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NumVerts * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	// First attrib, position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::setupTextures(const Shader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int heightNr = 1;

	for (int i = 0; i < textures.size(); i++) {
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
	}
}

void Mesh::Draw(const Shader& shader) {
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawElements(GL_TRIANGLES, m_NumVerts, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}