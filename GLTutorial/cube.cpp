#include "cube.h"

Cube::Cube() {
    setupCubeMesh();
}

Cube::Cube(const Shader& shader, std::string& texturePath) {
	const std::string directory = texturePath.substr(0, texturePath.find_last_of("/"));
	const std::string path = texturePath.substr( texturePath.find_last_of("/") + 1, texturePath.size());

	Texture tex = TextureLoader::loadTexture(path.c_str(), directory, "texture_diffuse");
    cubeTexture = tex.id;
    shader.bind();
    shader.addUniform1i(tex.type, cubeTexture);
    shader.unbind();

    hasTexture = true;
    setupCubeMesh();
}

void Cube::setupCubeMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void Cube::Draw(Shader& shader) {

    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0 + cubeTexture);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); 
    glBindVertexArray(0);
}