#pragma once

#include "common.h"
#include "shader/shader.h"
#include "model/skybox.h"
#include "model/model.h"
#include "util/camera.h"
#include "model/cube.h"  
#include "model/plane.h"  
#include "shader/light.h"  
#include "util/framebuffer.h"

#include <filesystem>
#include <memory>
#include <future>

constexpr unsigned int NUM_MODELS = 3;

constexpr glm::vec3 MODEL_POSITIONS[NUM_MODELS] = {
    glm::vec3(2.0f,10.0f,-1.0f),
    glm::vec3(-3.0f,5.0f,3.0f),
    glm::vec3(3.5f,2.0f,2.0f)
};

constexpr glm::vec3 SUN_LIGHT_DIR = glm::vec3(-1.0f,1.0f,1.0f);

const LightData POINT_LIGHT_DATA(
    glm::vec3(.1f),
    glm::vec3(.4f),
    glm::vec3(1.0f));

const LightData SUN_LIGHT_DATA(
    glm::vec3(.1f),
    glm::vec3(.9f),
    glm::vec3(0.0f));

inline constexpr unsigned int NUM_POINT_LIGHTS = 5;

const std::string WORKING_DIR = std::filesystem::current_path().generic_string();

static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float lastX = game_constants::SCR_WIDTH / 2.0f;
static float lastY = game_constants::SCR_HEIGHT / 2.0f;
static bool firstMouse = true;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static std::vector<glm::vec3> generatePositions(int numPointLights, std::vector<glm::mat4>& modelMatrices);