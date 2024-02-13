#pragma once
#include "common.h"
#include "shader/shader.h"
#include "util/textures.h"

class InstanceData {
public:
	InstanceData() = delete;
	void setup_instace_data(const Shader& shader);
private:
	unsigned int m_InstanceVBO;
	unsigned int m_NumInstances;
};

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class SceneEntity {
public:
	SceneEntity();
private:
	unsigned int m_VAO, m_VBO;

	std::unique_ptr<Texture> m_DepthTexture;
	std::unique_ptr<float> m_Vertices;
	Transform m_Transform;
	Shader m_Shader;
};