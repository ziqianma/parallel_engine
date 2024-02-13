#include "plane.h"

Plane::Plane(const Shader& shader, const std::string& texturePath, int depthMapTextureID)
    : m_DepthMapTexture("", depthMapTextureID, TextureType::TEXTURE_DEPTH, TextureLoader::GetAvailableTextureUnit(shader.get_shader_id(), type_name_map[TextureType::TEXTURE_DEPTH])),
    m_ShaderProgramID(shader.get_shader_id()),
    m_PlaneTexture(TextureLoader::LoadTexture(shader.get_shader_id(), texturePath, TextureType::TEXTURE_DIFFUSE))
{
    setup_material(shader);
    setup_mesh();
}

void Plane::Draw(const Shader& shader)
{
    shader.bind();
    shader.addUniformMat4("model", glm::mat4(1.0f));
    glActiveTexture(GL_TEXTURE0 + m_PlaneTexture.texUnit);
    glBindTexture(GL_TEXTURE_2D, m_PlaneTexture.id);
    
    glActiveTexture(GL_TEXTURE0 + m_DepthMapTexture.texUnit);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture.id);

    // draw mesh
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader.unbind();

    glBindVertexArray(0);
}

void Plane::setup_material(const Shader& shader)
{      
    Texture specularTexture = m_PlaneTexture;
    specularTexture.textureType = TextureType::TEXTURE_SPECULAR;

    Material material = Material(std::vector<Texture>{specularTexture, m_PlaneTexture, m_DepthMapTexture},
        glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f);

    shader.bind();
    shader.addUniformMaterial("material", material);
    shader.unbind();
}

Plane::~Plane() {
    TextureLoader::DeleteTexture(m_ShaderProgramID, m_PlaneTexture.path);

    glDeleteBuffers(1, &m_InstanceVBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Plane::setup_mesh()
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