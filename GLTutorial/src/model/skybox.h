#pragma once

#include "common.h"
#include "util/textures.h"
#include "shader/shader.h"
#include <stb_image.h>

class Skybox {
public:
	Skybox(const Shader& shader) : m_Shader(shader) {
        stbi_set_flip_vertically_on_load(false);
        m_Texture = TextureLoader::LoadSkyboxTexture(shader.get_shader_id(), "resources/skybox", "texture_skybox");
		stbi_set_flip_vertically_on_load(true);

        shader.bind();
        shader.addUniform1i(m_Texture->type + "1", 0);
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

    void Draw(const glm::mat4& viewMatrix) {
        // set depth function to less than or equal (all skybox depth vales are 1.0)
        glDepthFunc(GL_LEQUAL);

        m_Shader.bind();
        m_Shader.addUniformMat4("view", glm::mat4(glm::mat3(viewMatrix)));

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0 + m_Texture->texUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture->id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_Shader.unbind();

        glDepthFunc(GL_LESS);
    }

private:
	unsigned int VAO, VBO;
	std::unique_ptr<Texture> m_Texture;
    Shader m_Shader;

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