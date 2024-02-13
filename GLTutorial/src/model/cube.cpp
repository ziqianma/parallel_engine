#include "cube.h"

Cube::Cube(const Shader& shader, const std::string& texturePath, const std::vector<glm::mat4>& modelMatrices) :
    m_VAO(0),
    m_VBO(0),
    m_InstanceVBO(0),
    m_NumInstances(modelMatrices.size()),
    m_ShaderProgramID(shader.get_shader_id()),
    m_CubeTexture(TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, TextureType::TEXTURE_DIFFUSE)),
    m_DepthMapTexture(nullptr)
{
    if (m_NumInstances != 0) {

        setup_material(shader);

        setup_mesh();
        setup_instances(modelMatrices);
    }
}

Cube::Cube(const Shader& shader, const std::string& texturePath, const std::vector<glm::mat4>& modelMatrices, unsigned int depthMapTextureID) :
    m_VAO(0),
    m_VBO(0),
    m_InstanceVBO(0),
    m_NumInstances(modelMatrices.size()),
    m_ShaderProgramID(shader.get_shader_id()),
    m_CubeTexture(TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, TextureType::TEXTURE_DIFFUSE)),
    m_DepthMapTexture(std::make_unique<Texture>("", depthMapTextureID, TextureType::TEXTURE_DEPTH, TextureLoader::GetAvailableTextureUnit(shader.get_shader_id(), type_name_map[TextureType::TEXTURE_DEPTH])))
{
    if (m_NumInstances != 0) {
        setup_mesh();
        setup_instances(modelMatrices);

        setup_material(shader);
    }
}

Cube::~Cube() {
    TextureLoader::DeleteTexture(m_ShaderProgramID, m_CubeTexture.path);
}


void Cube::setup_material(const Shader& shader)
{
    Texture specularTexture = m_CubeTexture;
    specularTexture.textureType = TextureType::TEXTURE_SPECULAR;

    std::vector<Texture> textureList = std::vector<Texture>{ specularTexture, m_CubeTexture };
    if (m_DepthMapTexture) {
        textureList.push_back(*m_DepthMapTexture);
    }

    Material material = Material(textureList, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 128.0f);

    shader.bind();
    shader.addUniformMaterial("material", material);
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
    shader.bind();
    shader.addUniformMat4("model", glm::mat4(1.0f));

    glActiveTexture(GL_TEXTURE0 + m_CubeTexture.texUnit);
    glBindTexture(GL_TEXTURE_2D, m_CubeTexture.id);

    if (m_DepthMapTexture) {
        glActiveTexture(GL_TEXTURE0 + m_DepthMapTexture->texUnit);
        glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture->id);
    }

    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, m_NumInstances);
    glBindVertexArray(0);
    shader.unbind();
}