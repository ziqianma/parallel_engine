#include "light.h"

LightManager::LightManager(const Shader& shader, DirLight sun, const std::vector<glm::vec3>& positions, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
    m_Sun(sun)
{
    m_PointLights.reserve(positions.size());
    for(glm::vec3 position : positions) {
        m_PointLights.emplace_back(shader, position, ambient, diffuse, specular);
    }
}


void DirLight::setupLight(const Shader& shader) {
    shader.bind();
    shader.addUniform3f(m_Name + ".ambient", m_Light.ambient.r, m_Light.ambient.g, m_Light.ambient.b);
    shader.addUniform3f(m_Name + ".diffuse", m_Light.diffuse.r, m_Light.diffuse.g, m_Light.diffuse.b);
    shader.addUniform3f(m_Name + ".specular", m_Light.specular.r, m_Light.specular.g, m_Light.specular.b);
    shader.addUniform3f(m_Name + ".direction", m_Direction.x, m_Direction.y, m_Direction.z);
    shader.unbind();
    
}

void PointLight::setupLight(const Shader& shader, int index) {


}