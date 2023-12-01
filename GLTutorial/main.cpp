#include "main.h"
#include "stb_image.h"  

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float dt = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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
    Shader ourShader = Shader::Shader("vertex.shader", "frag.shader");
    Shader lightShader = Shader::Shader("vertexLight.shader", "fragLight.shader");
    Shader framebufferShader = Shader::Shader("framebuffervertex.shader", "framebufferfrag.shader");
    Shader skyboxShader = Shader::Shader("skyboxVertex.shader", "skyboxFrag.shader");

    glUseProgram(ourShader.createShaderProgram());
    skyboxShader.addUniform1i("skybox", 0);

    glUseProgram(skyboxShader.createShaderProgram());
    skyboxShader.addUniform1i("skyboxTexture", 0);

    glUseProgram(framebufferShader.createShaderProgram());
    framebufferShader.addUniform1i("screenTexture", 0);

    std::string workingDir = std::filesystem::current_path().generic_string();
    Model ourModel(workingDir + "/" + "resources/model/chair/chair.obj");

    std::string cubeTexturePath = workingDir + "/" + "resources/redstone_lamp_on.png";
    Cube lightCube = Cube::Cube(cubeTexturePath);


    // attach skybox texture
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(("resources/skybox/" + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // create skybox vao
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

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

        // render
        // ------   

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        DrawScene(lightShader, ourShader, lightCube, ourModel); // render actual scene

        // set depth function to less than or equal (all skybox depth vales are 1.0)
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShader.createShaderProgram());

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove camera translation from view matrix in skybox (we just want rotation/scale)
        skyboxShader.addUniformMat4("view", view);
        skyboxShader.addUniformMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

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

void DrawScene(Shader& lightShader, Shader& ourShader, Cube &lightCube, Model &ourModel) {
    glUseProgram(lightShader.createShaderProgram());
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    glm::vec3 lightMove = glm::vec3(sin(glfwGetTime()), 0, 0);


    lightShader.addUniformMat4("view", view);
    lightShader.addUniformMat4("projection", projection);

    
    for (glm::vec3 vec : pointLightPositions) {
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));

        model = glm::translate(model, vec + lightMove);
        lightShader.addUniformMat4("model", model);
        lightCube.Draw(lightShader);
    }
    

    // don't forget to enable shader before setting uniforms
    glUseProgram(ourShader.createShaderProgram());
    ourShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

    for (int i = 0; i < 4; i++) {
        std::string num = std::to_string(i);

        ourShader.addUniform3f("pointLights[" + num + "].ambient", 0.1f, 0.1f, 0.1f);
        ourShader.addUniform3f("pointLights[" + num + "].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.addUniform3f("pointLights[" + num + "].specular", 1.0f, 1.0f, 1.0f);
        ourShader.addUniform3f("pointLights[" + num + "].position", pointLightPositions[i].x + lightMove.x, pointLightPositions[i].y + lightMove.y, pointLightPositions[i].z + lightMove.z);
        ourShader.addUniform1f("pointLights[" + num + "].constant", 1.0f);
        ourShader.addUniform1f("pointLights[" + num + "].linear", 0.09f);
        ourShader.addUniform1f("pointLights[" + num + "].quadratic", 0.032f);
    }

    // view/projection transformations
    model = glm::mat4(1.0f);
    ourShader.addUniformMat4("projection", projection);
    ourShader.addUniformMat4("view", view);

    // render the loaded model
    for (int i = 0; i < 2; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        ourShader.addUniformMat4("model", model);
        ourModel.Draw(ourShader);
    }

}

