#include "light.h"

void DirLight::setupLight(const Shader& shader) const {
    shader.bind();
    shader.addUniform3f(m_Name + ".ambient", m_Ambient.r, m_Ambient.g, m_Ambient.b);
    shader.addUniform3f(m_Name + ".diffuse", m_Diffuse.r, m_Diffuse.g, m_Diffuse.b);
    shader.addUniform3f(m_Name + ".specular", m_Specular.r, m_Specular.g, m_Specular.b);
    shader.addUniform3f(m_Name + ".direction", m_Direction.x, m_Direction.y, m_Direction.z);
    shader.unbind();
    
}

void PointLight::setupLight(const Shader& shader) const {
    shader.bind();
    std::string num = std::to_string(m_ShaderIndex);

    shader.addUniform3f("pointLights[" + num + "].ambient", m_Ambient.r, m_Ambient.g, m_Ambient.b);
    shader.addUniform3f("pointLights[" + num + "].diffuse", m_Diffuse.r, m_Diffuse.g, m_Diffuse.b);
    shader.addUniform3f("pointLights[" + num + "].specular", m_Specular.r, m_Specular.g, m_Specular.b);
    shader.addUniform3f("pointLights[" + num + "].position", m_Position.x, m_Position.y, m_Position.z);
    shader.addUniform1f("pointLights[" + num + "].constant", 1.0f);
    shader.addUniform1f("pointLights[" + num + "].linear", 0.09f);
    shader.addUniform1f("pointLights[" + num + "].quadratic", 0.032f);
    shader.unbind();
}