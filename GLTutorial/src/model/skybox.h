#pragma once

#include "common.h"
#include "util/textures.h"
#include "shader/shader.h"
#include <stb_image.h>

class Skybox {
public:
    Skybox(const Shader& shader) : m_Texture(nullptr) {
        stbi_set_flip_vertically_on_load(false);
        m_Texture = std::make_unique<Texture>(TextureLoader::LoadSkyboxTexture(shader.get_shader_id(), "resources/skybox"));
        stbi_set_flip_vertically_on_load(true);

        if (m_Texture) {
            shader.bind();
            shader.addUniform1i(type_name_map[m_Texture->textureType] + "1", 0);
            shader.unbind();

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);
        }
        else {
            std::cerr << "Error, skybox texture failed to load" << std::endl;
        }
    }

    void Draw(const Shader& shader, const glm::mat4& viewMatrix) {
        // set depth function to less than or equal (all skybox depth vales are 1.0)
        glDepthFunc(GL_LEQUAL);

        shader.bind();
        shader.addUniformMat4("view", glm::mat4(glm::mat3(viewMatrix)));

        glBindVertexArray(VAO);

        if (m_Texture) {
            glActiveTexture(GL_TEXTURE0 + m_Texture->texUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture->id);
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.unbind();

        glDepthFunc(GL_LESS);
    }

private:
    unsigned int VAO, VBO;
    std::unique_ptr<Texture> m_Texture;

    float skyboxVertices[108] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
};