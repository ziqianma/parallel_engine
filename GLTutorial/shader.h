#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
    std::string _vertexShaderSourceCode;
    std::string _fragmentShaderSourceCode;
    std::vector<std::string> uniforms;
    unsigned int _shaderProgram;
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void addUniform4f(const std::string& name, float x, float y, float z, float w) const;
    void addUniform1i(const std::string& name, int x) const;
    void addUniform1f(const std::string& name, float x) const;
    void addUniformMat4(const std::string& name, glm::mat4 data);

    unsigned int createShaderProgram();
};