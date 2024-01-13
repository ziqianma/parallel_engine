#pragma once

#include "common.h"
#include "shader.h"
#include "skybox.h"
#include "model.h"
#include "camera.h"
#include "cube.h"  

#include <filesystem>
#include <memory>
#include <future>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 

glm::vec3 pointLightPositions[] = {
    glm::vec3(2.5f, -3.3f, 4.0f),
    glm::vec3(-2.0f, 2.0f, 12.0f),
    glm::vec3(5.0f,  0.0f, 3.0f)
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

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;