#pragma once

#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "model.h"
#include "camera.h"
#include "cube.h"  
#include "plane.h"  
#include "light.h"  

#include <filesystem>
#include <memory>
#include <future>

glm::vec3 modelPositions[] = {
    glm::vec3(2.5f, -3.3f, 4.0f),
    glm::vec3(-2.0f, 2.0f, 10.0f),
    glm::vec3(-5.0f,  0.0f, 3.0f),
    glm::vec3(-1.5f, -3.3f, 3.0f),
    glm::vec3(-2.0f, -2.0f, 6.0f),
    glm::vec3(4.0f,  0.0f, -3.0f),
    glm::vec3(-2.5f, -3.3f, 4.0f),
    glm::vec3(-2.0f, 2.0f, 12.0f),
    glm::vec3(3.5f,  0.0f, 2.0f)
};

float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

std::vector<std::string> faces
{
    "right.jpg",
    "left.jpg",
    "top.jpg",
    "bottom.jpg",
    "front.jpg",
    "back.jpg"
};

static const unsigned int SCR_WIDTH = 1920;
static const unsigned int SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 