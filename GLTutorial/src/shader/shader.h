#pragma once

#include "common.h"
#include "material.h"
#include <fstream>
#include <sstream>

class Shader {
private:
    std::string _vertexShaderSourceCode;
    std::string _fragmentShaderSourceCode;
    unsigned int m_ShaderProgram;
    mutable std::unordered_set<std::string> m_Uniforms;
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

    unsigned int get_shader_id() const;
    void bind() const;
    void unbind() const;
    void addUniform4f(const std::string& name, float x, float y, float z, float w) const;
    void addUniform3f(const std::string& name, float x, float y, float z) const;
    void addUniform1i(const std::string& name, int x) const;
    void addUniform1f(const std::string& name, float x) const;
    void addUniformMat4(const std::string& name, const glm::mat4& data) const;
    void addUniformMat3(const std::string& name, const glm::mat3& data) const;

    void addUniformMaterial(const std::string& name, const Material& material) const;

    void createShaderProgram();
};