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

#define NUM_MODELS 1

const static glm::vec3 MODEL_POSITIONS[NUM_MODELS] = {
    glm::vec3(0.0f,-10.0f,0.0f)
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