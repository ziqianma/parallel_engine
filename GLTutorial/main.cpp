#include "main.h"
#include <stb_image.h>

#define NUM_LIGHTS 3

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

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

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
    Shader skyboxShader = Shader::Shader("skyboxVertex.shader", "skyboxFrag.shader");

    std::string workingDir = std::filesystem::current_path().generic_string();

    glm::vec3 pointLightAmbient(.2f);
    glm::vec3 pointLightDiffuse(.5f);
    glm::vec3 pointLightSpecular(1.0f);

    ourShader.bind();
    ourShader.addUniform1i("numPointLights", NUM_LIGHTS);
    for (int i = 0; i < NUM_LIGHTS; i++) {
        std::string num = std::to_string(i);

        ourShader.addUniform3f("pointLights[" + num + "].ambient", pointLightAmbient.r, pointLightAmbient.g, pointLightAmbient.b);
        ourShader.addUniform3f("pointLights[" + num + "].diffuse", pointLightDiffuse.r, pointLightDiffuse.g, pointLightDiffuse.b);
        ourShader.addUniform3f("pointLights[" + num + "].specular", pointLightSpecular.r, pointLightSpecular.g, pointLightSpecular.b);
        ourShader.addUniform3f("pointLights[" + num + "].position", pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
        ourShader.addUniform1f("pointLights[" + num + "].constant", 1.0f);
        ourShader.addUniform1f("pointLights[" + num + "].linear", 0.09f);
        ourShader.addUniform1f("pointLights[" + num + "].quadratic", 0.032f);
    }
    ourShader.unbind();

    Skybox skybox(skyboxShader, faces);

    // load and attach model/light textures
    Model ourModel(workingDir + "/" + "resources/model/backpack/backpack.obj", ourShader);
    Cube lightCube(lightShader, workingDir + "/" + "resources/redstone_lamp_on.png");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    ourShader.bind();
    ourShader.addUniformMat4("projection", projection);
    ourShader.unbind();

    skyboxShader.bind();
    skyboxShader.addUniformMat4("projection", projection);
    skyboxShader.unbind();

    lightShader.bind();
    lightShader.addUniformMat4("projection", projection);
    lightShader.unbind();

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

        glm::mat4 view = camera.GetViewMatrix();

        // render
        // ------   
        
        TextureLoader::Update();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_DEPTH_TEST);
        DrawScene(lightShader, ourShader, lightCube, view); // render actual scene

        ourShader.bind();
        ourShader.addUniformMat4("view", view);
        ourShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.addUniformMat4("model", model);
        ourModel.Draw(ourShader);
        ourShader.unbind();

        skybox.Draw(view);

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

void DrawScene(Shader& lightShader, Shader& ourShader, Cube& lightCube, glm::mat4 view) {
    glm::mat4 model = glm::mat4(1.0f);

    lightShader.bind();
    lightShader.addUniformMat4("view", view);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));

        model = glm::translate(model, pointLightPositions[i]);
        lightShader.addUniformMat4("model", model);
        lightCube.Draw(lightShader);
    }
    lightShader.unbind();
}