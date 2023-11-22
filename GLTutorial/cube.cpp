#include "cube.h"

Cube::Cube() {
    setupCubeMesh();
}

Cube::Cube(std::string& texturePath) {
	const std::string directory = texturePath.substr(0, texturePath.find_last_of("/"));
	const std::string path = texturePath.substr( texturePath.find_last_of("/"), texturePath.size());

	cubeTexture = TextureLoader::loadTexture(path.c_str(), directory, "texture_diffuse");
    
    setupCubeMesh();
}

void Cube::setupCubeMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
	
}

void Cube::Draw(Shader& shader) {
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}