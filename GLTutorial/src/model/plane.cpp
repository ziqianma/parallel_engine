#include "plane.h"

Plane::Plane(const Shader& shader, const std::string& texturePath, const glm::mat4& modelMatrix)
{
    loadPlaneTexture(shader, texturePath);
    setupPlaneMesh();

    shader.bind();
    shader.addUniformMat4("model", modelMatrix);
    shader.addUniform3f("normal", 0.0f, 1.0f, 0.0f);
    shader.unbind();
}

void Plane::Draw(const Shader& shader)
{
    shader.bind();
    glActiveTexture(GL_TEXTURE0 + m_PlaneTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_PlaneTextureID);
    
    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    shader.unbind();
}

void Plane::loadPlaneTexture(const Shader& shader, const std::string& texturePath)
{
    TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, "texture_diffuse");

    const Texture& temp = TextureLoader::GetTexture(texturePath);
    m_PlaneTextureID = temp.id;
    m_PlaneTextureUnit = temp.texUnit;

    shader.bind();
    shader.addUniform1i("material.texture_specular1", m_PlaneTextureUnit);
    shader.addUniform1i("material.texture_diffuse1", m_PlaneTextureUnit);

    shader.addUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.specular", 1.0f, 1.0f, 1.0f);
    shader.addUniform1f("material.shininess", 4.0f);
    shader.unbind();
}

void Plane::setupPlaneMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_VERTS), PLANE_VERTS, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}