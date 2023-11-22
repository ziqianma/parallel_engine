#pragma once
#include "common.h"
#include "shader.h"
#include "mesh.h"
#include "textures.h"

class Cube {
public:
    Cube();
    Cube(std::string& texturePath);
    void Draw(Shader &shader);
private:
    Texture cubeTexture;
    unsigned int VAO, VBO, EBO;
    float cubeVertices[24] {
        -1, -1,  0.5, //0
            1, -1,  0.5, //1
        -1,  1,  0.5, //2
            1,  1,  0.5, //3
        -1, -1, -0.5, //4
            1, -1, -0.5, //5
        -1,  1, -0.5, //6
            1,  1, -0.5  //7
    };
    unsigned int cubeIndices[36] {
        //Top
        2, 6, 7,
        2, 3, 7,

        //Bottom
        0, 4, 5,
        0, 1, 5,

        //Left
        0, 2, 6,
        0, 4, 6,

        //Right
        1, 3, 7,
        1, 5, 7,

        //Front
        0, 2, 3,
        0, 1, 3,

        //Back
        4, 6, 7,
        4, 5, 7
    };
    unsigned int textureID = -1;
    void setupCubeMesh();
};