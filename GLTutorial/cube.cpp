#include "cube.h"

Cube::Cube() {
    setupCubeMesh();
}

Cube::Cube(const Shader& shader, const std::string& texturePath) {
	m_CubeTexture = TextureLoader::LoadTexture(texturePath, "texture_diffuse");

    shader.bind();
    shader.addUniform1i(m_CubeTexture.type, m_CubeTexture.texUnit);
    shader.unbind();

    hasTexture = true;
    setupCubeMesh();
}

void Cube::setupCubeMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void Cube::Draw(Shader& shader) {

    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0 + m_CubeTexture.texUnit);
        glBindTexture(GL_TEXTURE_2D, m_CubeTexture.id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); 
    glBindVertexArray(0);
}