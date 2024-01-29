#include "mesh.h"

// Constrctor intializes mesh data along with VAO, VBO and EBO

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<std::string>& texturePaths, unsigned int numVerts, unsigned int numInstances) :
	m_NumVerts(numVerts),
	m_NumInstances(numInstances)
{
	setupMesh(vertices, indices);

	for (const std::string& texturePath : texturePaths) {
		const Texture& temp = TextureLoader::GetTexture(texturePath);
		m_TextureIDs.push_back(temp.id);
		m_TextureUnits.push_back(temp.texUnit);
	}
}

void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	//AutoProfiler profiler("Mesh::setupMesh()");
	// Generate VAO and VBO/EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind vertex attributes to VAO
	glBindVertexArray(VAO);

	// Bind VBO/EBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

void Mesh::Draw(const Shader& shader) {
	for (int i = 0; i < m_TextureIDs.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + m_TextureUnits[i]);
		glBindTexture(GL_TEXTURE_2D, m_TextureIDs[i]);
	}

	shader.bind();
	// draw mesh
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawElementsInstanced(GL_TRIANGLES, m_NumVerts, GL_UNSIGNED_INT, 0, m_NumInstances);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	shader.unbind();
}
