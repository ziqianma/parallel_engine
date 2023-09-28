#include "main.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "camera.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float dt = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// light
float ambientIntensity = 0.1f;
glm::vec3 lightPos(1.2f, 0.5f, 1.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int main(void)  
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Error" << std::endl;


    std::cout << glGetString(GL_VERSION) << std::endl;
   
    TextureLoader loader;

    Texture texture = Texture::Texture(0, "redstone_lamp_on.png");
    loader.addTexture(0, texture);
    loader.loadTexture(0, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST);

    Texture texture1 = Texture::Texture(1, "container2.png");
    loader.addTexture(1, texture1);
    loader.loadTexture(1, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST);

    Texture texture2 = Texture::Texture(2, "container2_specular.png");
    loader.addTexture(2, texture2);
    loader.loadTexture(2, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST);

    Texture texture3 = Texture::Texture(3, "matrix.jpg");
    loader.addTexture(3, texture3);
    loader.loadTexture(3, GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST);

    // create shader
    Shader shaderProgramObj = Shader::Shader("vertex.shader", "frag.shader");
    unsigned int shaderProgram = shaderProgramObj.createShaderProgram();

    shaderProgramObj.addUniform1f("material.shininess", 32.0f);

    shaderProgramObj.addUniform1i("material.diffuse", 1);
    shaderProgramObj.addUniform1i("material.specular", 2);
    shaderProgramObj.addUniform1i("material.emission", 3);

    Shader shaderProgramLightObj = Shader::Shader("vertexLight.shader", "fragLight.shader");
    unsigned int shaderProgramLight = shaderProgramLightObj.createShaderProgram();
    shaderProgramLightObj.addUniform1i("texture1", 0);

    VAO vao = VAO::VAO(std::vector<float>(vertices, vertices + sizeof(vertices) / sizeof(vertices[0])), std::vector<int>({ 3, 3, 2 }), 8);
    VAO lightVao = VAO::VAO(std::vector<float>(vertices, vertices + sizeof(vertices) / sizeof(vertices[0])), std::vector<int>({ 3, 3, 2 }), 8);

    /* Loop until the user closes the window */ 
    while (!glfwWindowShouldClose(window))
    {   
        float currentFrame = static_cast<float>(glfwGetTime());
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)   
            camera.ProcessKeyboard(FORWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, dt);

        /* Render here */
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        loader.bindTextures();

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();
        shaderProgramObj.addUniformMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderProgramObj.addUniformMat4("projection", projection);
        shaderProgramObj.addUniform1f("u_time", (float)glfwGetTime());
       

        shaderProgramObj.addUniform3f("light.position", lightPos.x, lightPos.y, lightPos.z);
        shaderProgramObj.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        shaderProgramLightObj.addUniform3f("lightColor", lightColor.x, lightColor.y, lightColor.z);

        glm::vec3 diffuseColor = lightColor;
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        shaderProgramObj.addUniform3f("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
        shaderProgramObj.addUniform3f("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z); // darken diffuse light a bit
        shaderProgramObj.addUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

        shaderProgramObj.addUniform1f("light.kc", 1.0f);
        shaderProgramObj.addUniform1f("light.kl", 0.09f);
        shaderProgramObj.addUniform1f("light.kq", 0.032f);

        glBindVertexArray(vao.getId());
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderProgramObj.addUniformMat4("model", model);
            shaderProgramObj.addUniformMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Render the light 
        glUseProgram(shaderProgramLight);
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));

        shaderProgramLightObj.addUniformMat4("model", lightModel);
        shaderProgramLightObj.addUniformMat4("view", view);
        shaderProgramLightObj.addUniformMat4("projection", projection);

        glBindVertexArray(lightVao.getId());
        glDrawArrays(GL_TRIANGLES, 0, 36);
            
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}