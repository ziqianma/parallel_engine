#pragma once
#include "common.h"
#include "util/textures.h"
#include "shader/shader.h"

struct Material {
	std::vector<Texture> textures;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(const std::vector<Texture>& textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) :
		textures(textures), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
};