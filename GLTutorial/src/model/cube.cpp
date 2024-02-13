#include "cube.h"

Cube::Cube(const Shader& shader, const std::vector<glm::mat4>& modelMatrices) :
    m_ShaderProgramID(shader.get_shader_id()),
    m_NumInstances(modelMatrices.size()),
    m_InstanceVBO(0),
    m_VAO(0),
    m_VBO(0),
    m_DepthMapTextureID(0),
    m_DepthMapTextureUnit(0),
    m_CubeTexture(nullptr)
{
    if (m_NumInstances != 0) {
        bind_uniforms(shader);

        setup_mesh();
        setup_instances(modelMatrices);
    }
}

Cube::~Cube() {
    if (m_CubeTexture) {
        TextureLoader::DeleteTexture(m_ShaderProgramID, m_CubeTexture->path);
    }
}

Cube::Cube(const Shader& shader, const std::string& texturePath, const std::vector<glm::mat4>& modelMatrices) :
    m_ShaderProgramID(shader.get_shader_id()), 
    m_NumInstances(modelMatrices.size()),
    m_InstanceVBO(0),
    m_VAO(0),
    m_VBO(0),
    m_DepthMapTextureID(0),
    m_DepthMapTextureUnit(0),
    m_CubeTexture(std::make_unique<Texture>(TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, TextureType::TEXTURE_DIFFUSE)))
{
    if (m_NumInstances != 0) {
        bind_uniforms(shader);

        setup_mesh();
        setup_instances(modelMatrices);
    }
}

Cube::Cube(const Shader& shader, const std::string& texturePath, const std::vector<glm::mat4>& modelMatrices, unsigned int depthMapTextureID) :
    Cube(shader, texturePath, modelMatrices)
{
    m_DepthMapTextureID = depthMapTextureID;
    m_DepthMapTextureUnit = TextureLoader::GetAvailableTextureUnit(shader.get_shader_id(), "shadow_map");

    shader.bind();
    shader.addUniform1i("shadow_map", m_DepthMapTextureUnit);
    shader.unbind();
}

void Cube::bind_uniforms(const Shader& shader)
{
    shader.bind();
    shader.addUniformMat4("model", glm::mat4(1.0f));

    shader.addUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.addUniform3f("material.specular", 1.0f, 1.0f, 1.0f);
    shader.addUniform1f("material.shininess", 128.0f);

    if (m_CubeTexture) {
        shader.addUniform1i("material.texture_diffuse1", m_CubeTexture->texUnit);
        shader.addUniform1i("material.texture_specular1", m_CubeTexture->texUnit);
    }

    shader.unbind();
}


void Cube::setup_mesh() 
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

void Cube::setup_instances(std::vector<glm::mat4> modelMatrices)
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
    if (m_CubeTexture) {
        glActiveTexture(GL_TEXTURE0 + m_CubeTexture->texUnit);
        glBindTexture(GL_TEXTURE_2D, m_CubeTexture->id);
    }

    glActiveTexture(GL_TEXTURE0 + m_DepthMapTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapTextureID);

    shader.bind();
    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, m_NumInstances);
    glBindVertexArray(0);
    shader.unbind();
}