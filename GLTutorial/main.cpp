#include "main.h"
#include <stb_image.h>

std::string workingDir = std::filesystem::current_path().generic_string();

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float dt = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int frameCount = 0;

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
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    // create shader
    Shader ourShader = Shader::Shader("shaders/vertex.shader", "shaders/frag.shader");
    Shader cubeShader = Shader::Shader("shaders/vertexCube.shader", "shaders/fragCube.shader");
    Shader skyboxShader = Shader::Shader("shaders/skyboxVertex.shader", "shaders/skyboxFrag.shader");

    int numPointLights = 3;

    ourShader.bind();
    ourShader.addUniform1i("numPointLights", numPointLights);
    ourShader.unbind();

    DirLight sun(ourShader, glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(.0f), glm::vec3(.0f, 1.0f, .0f), glm::vec3(.0f), "sun");
    std::vector<PointLight> pointLights;

    glm::vec3 pointLightAmbient(.1f);       
    glm::vec3 pointLightDiffuse(.6f);
    glm::vec3 pointLightSpecular(1.0f);

    for (int i = 0; i < numPointLights; i++) {
        pointLights.emplace_back(ourShader, i, pointLightPositions[i], pointLightAmbient, pointLightDiffuse, pointLightSpecular);
    }

    Skybox skybox(skyboxShader, faces); 

    std::vector<glm::mat4> modelMatrices;

    unsigned int amount = 500;
    srand(glfwGetTime()); // initialize random seed	
    float radius = 10.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices.push_back(model);
    }

    // load and attach model/light textures
    Model* ourModel = new Model(workingDir + "/" + "resources/model/backpack/backpack.obj", ourShader, modelMatrices);
    Cube lightCube(cubeShader, workingDir + "/" + "resources/redstone_lamp_on.png");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameCount++;
        if (frameCount > 500) {
            std::cout << "FPS: " << 1 / dt << std::endl;
            frameCount = 0;
        }
        

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

        
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            delete ourModel;
            ourModel = nullptr;
        }
        

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        skyboxShader.bind();
        skyboxShader.addUniformMat4("projection", projection);
        skyboxShader.unbind();
                
        cubeShader.bind();
        cubeShader.addUniformMat4("projection", projection);
        cubeShader.unbind();

        glm::mat4 view = camera.GetViewMatrix();

        // render
        // ------   
        
        TextureLoader::Update();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_DEPTH_TEST);
        //render lights
        glm::mat4 lightModel = glm::mat4(1.0f);

        cubeShader.bind();
        cubeShader.addUniformMat4("view", view);

        for (int i = 0; i < numPointLights; i++) {
            lightModel = glm::mat4(1.0f);
            lightModel = glm::scale(lightModel, glm::vec3(0.5f));

            lightModel = glm::translate(lightModel, pointLightPositions[i]);
            cubeShader.addUniformMat4("model", lightModel);
            lightCube.Draw(cubeShader);
        }
        cubeShader.unbind();

        
        if (ourModel) {
            ourShader.bind();
            // render model
            ourShader.addUniformMat4("projection", projection);
            ourShader.addUniformMat4("view", view);
            ourShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
            ourShader.unbind();

            ourModel->Draw(ourShader);
        }
        
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