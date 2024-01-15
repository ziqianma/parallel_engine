#pragma once
#define MAX_POINT_LIGHTS 128

#include "common.h"
#include "shader.h"

class Light {
public:
	Light(const Shader &shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		m_Ambient(ambient),
		m_Diffuse(diffuse),
		m_Specular(specular) 
	{}
protected:
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	virtual void setupLight(const Shader& shader) const = 0;
};
  
class DirLight : public Light {
public:
	DirLight(const Shader& shader, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, const std::string& name) :
		Light(shader, ambient, diffuse, specular),
		m_Direction(direction),
		m_Name(name)
	{
		setupLight(shader);
	}
private:
	std::string m_Name;
	glm::vec3 m_Direction;

	void setupLight(const Shader& shader) const override;
};

class PointLight : public Light {
public:
	PointLight(const Shader& shader, unsigned int shaderIndex, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		Light(shader, ambient, diffuse, specular),
		m_Position(position),
		m_ShaderIndex(shaderIndex)
	{
		setupLight(shader);
	}

	void setupLight(const Shader & shader) const override;
private:
	glm::vec3 m_Position;
	unsigned int m_ShaderIndex;

};
