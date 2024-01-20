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

inline constexpr glm::vec3 MODEL_POSITIONS[NUM_MODELS] = {
    glm::vec3(0.0f,-10.0f,0.0f)
};

inline Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

inline float lastX = game_constants::SCR_WIDTH / 2.0f;
inline float lastY = game_constants::SCR_HEIGHT / 2.0f;
inline bool firstMouse = true;

inline const std::string WORKING_DIR = std::filesystem::current_path().generic_string();

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);
inline void mouse_callback(GLFWwindow* window, double xpos, double ypos);
inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);