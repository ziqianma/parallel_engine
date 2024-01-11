#include "mesh.h"

// Constrctor intializes mesh data along with VAO, VBO and EBO

Mesh::Mesh(Vertex* vertices, unsigned int* indices, const std::vector<Texture>& textures, unsigned int numVerts) :
	m_Vertices(vertices),
	m_Indices(indices),
	m_Textures(textures),
	m_NumVerts(numVerts)
{
	setupMesh();
}


void Mesh::setupMesh() {
	//AutoProfiler profiler("Mesh::setupMesh()");
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

	// Now that the vertex and index data is uploaded to GPU, we can delete them.
	delete[] m_Vertices;
	delete[] m_Indices;
}

void Mesh::Draw(const Shader& shader) {
	for (int i = 0; i < m_Textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + m_Textures[i].texUnit);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
	}

	shader.bind();
	// draw mesh
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawElements(GL_TRIANGLES, m_NumVerts, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	shader.unbind();
}
