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
    
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
   
    // create shader
    Shader shaderProgramObj = Shader::Shader("vertex.shader", "frag.shader");
    unsigned int shaderProgram = shaderProgramObj.createShaderProgram();
    shaderProgramObj.addUniform1i("texture1", 0);

    Shader shaderProgramLightObj = Shader::Shader("vertexLight.shader", "fragLight.shader");
    unsigned int shaderProgramLight = shaderProgramLightObj.createShaderProgram();

    // texture
    TextureLoader loader;

    Texture texture1 = Texture::Texture(0, "container.jpg");
    loader.addTexture(0, texture1);
    loader.loadTexture(0);

    VAO vao = VAO::VAO(std::vector<float>(vertices, vertices + sizeof(vertices) / sizeof(vertices[0])), std::vector<int>({ 3, 2 }), 5);
    VAO lightVao = VAO::VAO(std::vector<float>(vertices, vertices + sizeof(vertices) / sizeof(vertices[0])), std::vector<int>({ 3, 2 }), 5);

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

        shaderProgramObj.addUniform4f("lightColor", 0.5f + sin((float)glfwGetTime()) / 2, 1.0f, 1.0f, 1.0f);
        shaderProgramObj.addUniform4f("objectColor", 1.0f, 1.0f, 1.0f, 1.0f);

        glm::mat4 view = camera.GetViewMatrix();
        shaderProgramObj.addUniformMat4("view", view);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderProgramObj.addUniformMat4("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        shaderProgramObj.addUniformMat4("model", model);

        glBindVertexArray(vao.getId());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Render the light 
        glUseProgram(shaderProgramLight);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        shaderProgramLightObj.addUniform4f("lightColor", 0.5f + sin((float)glfwGetTime()) / 2, 1.0f, 1.0f, 1.0f);
        shaderProgramLightObj.addUniformMat4("model", model);
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