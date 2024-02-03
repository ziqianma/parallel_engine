#include "cube.h"

Cube::Cube(const std::vector<glm::mat4>& modelMatrices) :
    m_CubeTextureID(-1),
    m_NumInstances(modelMatrices.size())
{
    setupCubeMesh();
    setupInstances(modelMatrices);
}

Cube::Cube(const Shader& shader, const std::string& texturePath, const std::vector<glm::mat4>& modelMatrices) :
    m_NumInstances(modelMatrices.size()),
    m_CubeTextureID(-1),
    m_InstanceVBO(0),
    m_VAO(0),
    m_VBO(0),
    m_CubeTextureUnit(0)
{
    if (m_NumInstances != 0) {
        loadCubeTexture(shader, texturePath);

        setupCubeMesh();
        setupInstances(modelMatrices);
    }
}

void Cube::loadCubeTexture(const Shader& shader, const std::string& texturePath) 
{
    TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, "texture_diffuse");

    const Texture& temp = TextureLoader::GetTexture(texturePath);
    m_CubeTextureID = temp.id;
    m_CubeTextureUnit = temp.texUnit;

    shader.bind();
    shader.addUniform1i("material.texture_diffuse1", m_CubeTextureUnit);
    shader.addUniform1i("material.texture_specular1", m_CubeTextureUnit);

    shader.addUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.specular", 1.0f, 1.0f, 1.0f);
    shader.addUniform1f("material.shininess", 128.0f);
    shader.unbind();
}


void Cube::setupCubeMesh() 
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Cube::setupInstances(std::vector<glm::mat4> modelMatrices)
{
    glGenBuffers(1, &m_InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);

    // per mesh instacing vertex attribs
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void Cube::Draw(const Shader& shader) 
{
    shader.bind();
    shader.addUniformMat4("model", glm::mat4(1.0f));
    bool hasTexture = m_CubeTextureID != -1;
    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0 + m_CubeTextureUnit);
        glBindTexture(GL_TEXTURE_2D, m_CubeTextureID);
    }

    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, m_NumInstances);
    glBindVertexArray(0);
    shader.unbind();
}