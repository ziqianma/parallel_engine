#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        _vertexShaderSourceCode = vShaderStream.str();
        _fragmentShaderSourceCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    createShaderProgram();
}

void Shader::addUniform4f(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(m_ShaderProgram, name.c_str()), x, y, z, w);
}

void Shader::addUniform3f(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(m_ShaderProgram, name.c_str()), x, y, z);
}

void Shader::addUniform1i(const std::string& name, int x) const {
    glUniform1i(glGetUniformLocation(m_ShaderProgram, name.c_str()), x);
}

void Shader::addUniform1f(const std::string& name, float x) const {
    glUniform1f(glGetUniformLocation(m_ShaderProgram, name.c_str()), x);
}

void Shader::addUniformMat4(const std::string& name, const glm::mat4& data) const {
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(data));
}

void Shader::addUniformMat3(const std::string& name, const glm::mat3& data) const {
    glUniformMatrix3fv(glGetUniformLocation(m_ShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(data));
}

void Shader::addUniformMaterial(const std::string& name, const Material& material) const
{
    for (const Texture& texture : material.textures) {
        addUniform1i(name + "." + type_name_map[texture.textureType] + "1", texture.texUnit);
    }

    addUniform3f(name + ".ambient", material.ambient.x, material.ambient.y, material.ambient.z);
    addUniform3f(name + ".diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
    addUniform3f(name + ".specular", material.specular.x, material.specular.y, material.specular.z);
    addUniform1f(name + ".shininess", material.shininess);
}

void Shader::bind() const {
    glUseProgram(m_ShaderProgram);
}

void Shader::unbind() const {
    glUseProgram(0);
}

unsigned int Shader::get_shader_id() const {
    return m_ShaderProgram;
}

void Shader::createShaderProgram() {
    int success;
    char infoLog[512];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vsrc = _vertexShaderSourceCode.c_str();
    glShaderSource(vertexShader, 1, &vsrc, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsrc = _fragmentShaderSourceCode.c_str();
    glShaderSource(fragmentShader, 1, &fsrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {   
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); 

    m_ShaderProgram = shaderProgram;
}