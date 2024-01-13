#pragma once
#include "common.h"
#include "shader.h"

#define MAX_POINT_LIGHTS 128

struct Light {
public:
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		ambient(ambient),
		diffuse(diffuse),
		specular(specular) {}

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};
  
class DirLight {
public:
	DirLight(const Shader& shader, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, const std::string& name) :
		m_Light(std::move(Light(ambient, diffuse, specular))),
		m_Direction(direction),
		m_Name(name)
	{
		setupLight(shader);
	}
private:
	std::string m_Name;
	Light m_Light;
	glm::vec3 m_Direction;

	void setupLight(const Shader& shader);
};

class PointLight {
	PointLight(const Shader& shader, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		m_Light(std::move(Light(ambient, diffuse, specular))),
		m_Position(position)
	{}

	void setupLight(const Shader & shader, int index);
private:
	Light m_Light;
	glm::vec3 m_Position;

};

class LightManager {
public:
	LightManager(const Shader& shader, DirLight sun, const std::vector<glm::vec3>& positions, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
private:
	DirLight m_Sun;
	std::vector<PointLight> m_PointLights;

	void setupLights(const Shader& shader);
};
