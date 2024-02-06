#include "light.h"

void DirLight::setup_light(const Shader& shader) const {
    shader.bind();
    shader.addUniform3f(m_Name + ".ambient", m_LightData.ambient.r, m_LightData.ambient.g, m_LightData.ambient.b);
    shader.addUniform3f(m_Name + ".diffuse", m_LightData.diffuse.r, m_LightData.diffuse.g, m_LightData.diffuse.b);
    shader.addUniform3f(m_Name + ".specular", m_LightData.specular.r, m_LightData.specular.g, m_LightData.specular.b);
    shader.addUniform3f(m_Name + ".direction", m_Direction.x, m_Direction.y, m_Direction.z);
    shader.unbind();
    
}

void PointLight::setup_light(const Shader& shader) const {
    shader.bind();
    std::string num = std::to_string(m_PointLightID);

    shader.addUniform3f(m_Name + "[" + num + "].ambient", m_LightData.ambient.r, m_LightData.ambient.g, m_LightData.ambient.b);
    shader.addUniform3f(m_Name + "[" + num + "].diffuse", m_LightData.diffuse.r, m_LightData.diffuse.g, m_LightData.diffuse.b);
    shader.addUniform3f(m_Name + "[" + num + "].specular", m_LightData.specular.r, m_LightData.specular.g, m_LightData.specular.b);
    shader.addUniform3f(m_Name + "[" + num + "].position", m_Position.x, m_Position.y, m_Position.z);

    shader.addUniform1f(m_Name + "[" + num + "].constant", m_Attenuation.x);
    shader.addUniform1f(m_Name + "[" + num + "].linear", m_Attenuation.y);
    shader.addUniform1f(m_Name + "[" + num + "].quadratic", m_Attenuation.z);
    shader.unbind();
}

LightGroup::LightGroup(const std::vector<Shader>& shaders, const LightData& pointLight, const std::vector<glm::vec3>& positions, const LightData& dirLight, const glm::vec3& direction)
{
    if (!positions.empty()) {
        m_LightList.reserve(1 + positions.size());
    }

    auto dirLightPtr = std::make_unique<DirLight>(direction, dirLight, std::string(game_constants::DIR_LIGHT_NAME));
    m_LightList.push_back(std::move(dirLightPtr));
    m_DirLightIndex = 0;

    for (glm::vec3 position : positions) {
        unsigned int id = get_unique_pointlight_id();
        auto pointLightPtr = std::make_unique<PointLight>(id - 1, position, pointLight, std::string(game_constants::POINT_LIGHT_LIST_NAME));
        m_LightList.push_back(std::move(pointLightPtr));
        m_PointLightIndexes.push_back(id);
    }

    for (const Shader& shader : shaders) {
        for (auto& light : m_LightList) {
            light->setup_light(shader);
        }

        shader.bind();
        shader.addUniform1i("numPointLights", positions.size());
        shader.unbind();
    }
}

void LightGroup::update_light_data(const std::vector<Shader>& shaders) {
    for (const Shader& shader : shaders) {
        for (auto& light : m_LightList) {
            light->setup_light(shader);
        }
    }
}