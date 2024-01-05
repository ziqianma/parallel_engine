#pragma once

#include "common.h"
#include <fstream>
#include <sstream>

class Shader {
private:
    std::string _vertexShaderSourceCode;
    std::string _fragmentShaderSourceCode;
    std::vector<std::string> uniforms;
    unsigned int _shaderProgram;
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void addUniform4f(const std::string& name, float x, float y, float z, float w) const;
    void addUniform3f(const std::string& name, float x, float y, float z) const;
    void addUniform1i(const std::string& name, int x) const;
    void addUniform1f(const std::string& name, float x) const;
    void addUniformMat4(const std::string& name, glm::mat4 data) const;
    void addUniformMat3(const std::string& name, glm::mat3 data) const;

    unsigned int createShaderProgram();
};