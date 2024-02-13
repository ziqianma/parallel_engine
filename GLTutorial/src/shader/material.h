#pragma once
#include "common.h"
#include "util/textures.h"

struct Material {
	std::unique_ptr<Texture> ambient_map;
	std::unique_ptr<Texture> diffuse_map;
	std::unique_ptr<Texture> specular_map;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};