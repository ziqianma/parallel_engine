#include "cube.h"

Cube::Cube() {
    cubeTextureID = -1;
    setupCubeMesh();
}

Cube::Cube(const Shader& shader, const std::string& texturePath) {
    TextureLoader::LoadTexture(texturePath, "texture_diffuse");

    const Texture& temp = TextureLoader::GetTexture(texturePath);
    cubeTextureID = temp.id;
    cubeTextureUnit = temp.texUnit;

    shader.bind();
    shader.addUniform1i(temp.type, cubeTextureUnit);
    shader.unbind();

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

void Cube::Draw(const Shader& shader) {

    if (cubeTextureID != -1) {
        glActiveTexture(GL_TEXTURE0 + cubeTextureUnit);
        glBindTexture(GL_TEXTURE_2D, cubeTextureID);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); 
    glBindVertexArray(0);
}