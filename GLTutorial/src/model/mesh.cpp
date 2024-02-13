#include "mesh.h"

// Constrctor intializes mesh data along with VAO, VBO and EBO

Mesh::Mesh(const Shader& shader, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const UnloadedTextureList& textures, unsigned int numVerts, unsigned int numInstances, unsigned int depthTextureID, unsigned int depthTextureUnit) :
	m_NumVerts(numVerts),
	m_NumInstances(numInstances),
	m_DepthTextureID(depthTextureID),
	m_DepthTextureUnit(depthTextureUnit)
{
	setup_mesh(vertices, indices);
	load_textures(shader, textures);
}

void Mesh::load_textures(const Shader& shader, const UnloadedTextureList& textures) {
	// Load texture from loader
	for (const auto& [path, typeIndex] : textures) {
		const Texture& loaded = TextureLoader::LoadTexture(shader.get_shader_id(), path, static_cast<TextureType>(typeIndex));

		shader.bind();
		shader.addUniform1i(("material." + type_name_map[typeIndex] + "1"), loaded.texUnit);
		shader.unbind();

		m_Textures.emplace_back(loaded);
	}
}

void Mesh::setup_mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
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
	for (const Texture& texture : m_Textures) {
		glActiveTexture(GL_TEXTURE0 + texture.texUnit);
		glBindTexture(GL_TEXTURE_2D, texture.id);
	}

	glActiveTexture(GL_TEXTURE0 + m_DepthTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_DepthTextureID);

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
