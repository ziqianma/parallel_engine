#pragma once

#include "shader/shader.h"
#include "mesh.h"
#include "common.h"
#include "util/textures.h"
#include "util/framebuffer.h"

static const float PLANE_VERTS[] = {
    // positions                                    // texCoords
         25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};

class Plane {
public:
    Plane(const Shader& shader, const std::string& texturePath, int depthMapTextureID);
    ~Plane();
    void Draw(const Shader& shader);
private:
    unsigned int m_VAO, m_VBO;
    unsigned int m_InstanceVBO;

    unsigned int m_PlaneTextureID;
    unsigned int m_PlaneTextureUnit;

    unsigned int m_DepthMapTextureID;
    unsigned int m_DepthMapTextureUnit;


    void setupPlaneMesh();
    void loadPlaneTexture(const Shader& shader, const std::string& texturePath);
};