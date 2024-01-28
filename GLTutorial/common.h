#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "timer.h"

inline namespace game_constants {
    inline constexpr float QUAD_VERTICES[] = 
    {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    inline constexpr unsigned int NUM_SKYBOX_FACES = 6;
    inline constexpr std::array<const char*, NUM_SKYBOX_FACES> SKYBOX_FACES(
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    });

    inline constexpr unsigned int SCR_WIDTH = 1920;
    inline constexpr unsigned int SCR_HEIGHT = 1080;

    inline constexpr const char* POINT_LIGHT_LIST_NAME = "pointLights";
    inline constexpr const char* DIR_LIGHT_NAME = "sun";
}