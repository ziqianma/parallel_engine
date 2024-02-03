#include "plane.h"

Plane::Plane(const Shader& shader, const std::string& texturePath, int depthMapTextureID) 
    : m_DepthMapTextureID(depthMapTextureID)
{

    loadPlaneTexture(shader, texturePath);
    setupPlaneMesh();
}

void Plane::Draw(const Shader& shader)
{
    shader.bind();
    shader.addUniformMat4("model", glm::mat4(1.0f));
    glActiveTexture(GL_TEXTURE0 + m_PlaneTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_PlaneTextureID);
    
    glActiveTexture(GL_TEXTURE0 + m_DepthMapTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapTextureID);

    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader.unbind();

    glBindVertexArray(0);
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

    m_DepthMapTextureUnit = TextureLoader::GetAvailableTextureUnit(shader.get_shader_id(), "plane_depth_map");
    shader.addUniform1i("shadowMap", m_DepthMapTextureUnit);

    shader.addUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.specular", 1.0f, 1.0f, 1.0f);
    shader.addUniform1f("material.shininess", 4.0f);
    shader.unbind();
}

Plane::~Plane() {
    glDeleteBuffers(1, &m_InstanceVBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Plane::setupPlaneMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_VERTS), PLANE_VERTS, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // upload uniform model matrix as "instance data" for plane's VAO

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glGenBuffers(1, &m_InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(modelMatrix), &modelMatrix, GL_STATIC_DRAW);

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