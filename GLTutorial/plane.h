#pragma once

#include "common.h"
#include "shader.h"
#include "mesh.h"
#include "textures.h"

static const float PLANE_VERTS[] = {
    // positions         // texCoords
    -1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
     1.0f, 0.0f, -1.0f,  1.0f, 0.0f,

    -1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
     1.0f, 0.0f, -1.0f,  1.0f, 0.0f,
     1.0f, 0.0f, 1.0f,   1.0f, 1.0f
};


class Plane {
public:
    Plane(const Shader& shader, const std::string& texturePath, const glm::mat4& modelMatrix);
    void Draw(const Shader& shader);
private:
    unsigned int m_VAO, m_VBO;

    unsigned int m_InstanceVBO;

    int m_PlaneTextureID;
    unsigned int m_PlaneTextureUnit;

    void setupPlaneMesh();
    void loadPlaneTexture(const Shader& shader, const std::string& texturePath);
    void setupInstances(const std::vector<glm::mat4> modelMatrices);
};