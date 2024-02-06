#pragma once
#define MAX_POINT_LIGHTS 128

#include "common.h"
#include "shader.h"

inline unsigned int get_unique_pointlight_id() {
	static unsigned int id = 0;
	return ++id;
}

struct LightData {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	LightData(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		ambient(ambient),
		diffuse(diffuse),
		specular(specular)
	{}
};

class Light {
public:
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		m_LightData(ambient, diffuse, specular)
	{}

	Light(const LightData& lightData) :
		m_LightData(lightData)
	{}

	void set_diffuse(const glm::vec3& diffuse) { m_LightData.diffuse = diffuse; }
protected:
	LightData m_LightData;
private:
	virtual void setup_light(const Shader& shader) const = 0;

	friend class LightGroup;
};
  
class DirLight : public Light {
public:
	DirLight(glm::vec3 direction, const LightData& light, const std::string& name) :
		Light(light),
		m_Direction(direction),
		m_Name(name)
	{}

	DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, const std::string& name) :
		DirLight(direction, LightData(ambient, diffuse, specular), name)
	{}
private:
	std::string m_Name;
	glm::vec3 m_Direction;

	void setup_light(const Shader& shader) const override;

	friend class LightGroup;
};

class PointLight : public Light {
public:
	PointLight(unsigned int id, glm::vec3 position, const LightData& lightData, const std::string& name, float constant, float linear, float quadratic) :
		Light(lightData),
		m_Position(position),
		m_Name(name),
		m_PointLightID(id),
		m_Attenuation(constant, linear, quadratic)
	{}

	PointLight(unsigned int id, glm::vec3 position, const LightData& lightData, const std::string& name) :
		PointLight(id, position, lightData, name, 1.0f, 0.09f, 0.032f)
	{}

	PointLight(unsigned int id, const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const std::string& name) :
		PointLight(id, position, LightData(ambient, diffuse, specular), name, 1.0f, 0.09f, 0.032f)
	{}

	PointLight(unsigned int id, const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const std::string& name, float constant, float linear, float quadratic) :
		PointLight(id, position, LightData(ambient, diffuse, specular), name, constant, linear, quadratic)
	{}
private:
	std::string m_Name;
	glm::vec3 m_Position;
	glm::vec3 m_Attenuation;
	unsigned int m_PointLightID;

	void setup_light(const Shader& shader) const override;
	friend class LightGroup;
};

class LightGroup {
public:
	LightGroup(const std::vector<Shader>& shaders,
		const LightData& pointLight,
		const std::vector<glm::vec3>& positions,
		const LightData& dirLight,
		const glm::vec3& direction);
	
	const Light& operator[](unsigned int index) const {
		return *(m_LightList[index].get());
	}

	Light& operator[](unsigned int index) {
		return *(m_LightList[index].get());
	}

	void update_light_data(const std::vector<Shader>& shaders);
private:
	std::string m_Name;

	std::vector<std::unique_ptr<Light>> m_LightList;
	std::vector<unsigned int> m_PointLightIndexes;
	unsigned int m_DirLightIndex;
};