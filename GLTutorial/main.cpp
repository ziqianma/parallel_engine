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
    Shader modelShader = Shader::Shader("shaders/vertex.shader", "shaders/frag.shader");
    Shader cubeShader = Shader::Shader("shaders/vertexCube.shader", "shaders/fragCube.shader");
    Shader skyboxShader = Shader::Shader("shaders/skyboxVertex.shader", "shaders/skyboxFrag.shader");
    Shader planeShader = Shader::Shader("shaders/vertexPlane.shader", "shaders/fragPlane.shader");

    DirLight sun(std::vector<Shader>({modelShader,planeShader}), glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(.0f), glm::vec3(.1f, .1f, .1f), glm::vec3(.0f), "sun");

    glm::vec3 pointLightAmbient(.1f);       
    glm::vec3 pointLightDiffuse(.4f);
    glm::vec3 pointLightSpecular(1.0f);

    std::vector<glm::mat4> cube_ModelMatrices;
    int numPointLights = 10;

    modelShader.bind();
    modelShader.addUniform1i("numPointLights", numPointLights);
    modelShader.unbind();

    planeShader.bind();
    planeShader.addUniform1i("numPointLights", numPointLights);
    planeShader.unbind();

    srand(glfwGetTime()); // initialize random seed	
    float radius = 5.0f;
    float offset = 2.5f;

    std::vector<PointLight> pointLights;
    pointLights.reserve(numPointLights);

    for (unsigned int i = 0; i < numPointLights; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)numPointLights * 360.0f;
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
        cube_ModelMatrices.push_back(model);

        // setup light locations to be where cubes are rendered
        pointLights.emplace_back(std::vector<Shader>({ modelShader,planeShader }), i, glm::vec3(x, y, z), pointLightAmbient, pointLightDiffuse, pointLightSpecular);
    }

    std::vector<glm::mat4> model_ModelMatrices;
    for (int i = 0; i < NUM_MODELS; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));
        model = glm::translate(model, MODEL_POSITIONS[i]);
        model_ModelMatrices.push_back(model);
    }

    glm::mat4 floor_ModelMatrix = glm::mat4(1.0f);
    floor_ModelMatrix = glm::translate(floor_ModelMatrix, glm::vec3(0.0f,-5.0f,0.0f));
    floor_ModelMatrix = glm::scale(floor_ModelMatrix, glm::vec3(20.0f));

    // Create skybox
    Skybox skybox(skyboxShader, faces);

    // Create other game objs
    Model* ourModel = new Model(workingDir + "/" + "resources/model/dragon/dragon.obj", modelShader, model_ModelMatrices);
    Cube lightCube(cubeShader, workingDir + "/" + "resources/redstone_lamp_on.png", cube_ModelMatrices);
    Plane floor(planeShader, workingDir + "/" + "resources/floor2.jpg", floor_ModelMatrix);

    float angle = 0;
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

        planeShader.bind();
        planeShader.addUniformMat4("projection", projection);
        planeShader.unbind();

        glm::mat4 view = camera.GetViewMatrix();

        // render
        // ------   
        
        TextureLoader::Update();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_DEPTH_TEST);

        //render lights
        cubeShader.bind();
        cubeShader.addUniformMat4("view", view);
        cubeShader.unbind();

        lightCube.Draw(cubeShader);

        // render floor
        planeShader.bind();
        planeShader.addUniformMat4("view", view);
        planeShader.unbind();

        floor.Draw(planeShader);

        // render model
        if (ourModel) {
            modelShader.bind();
            glm::mat4 model = glm::mat4(1.0f);
            angle += 0.1*dt;
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            modelShader.addUniformMat4("model", model);

            modelShader.addUniformMat4("projection", projection);
            modelShader.addUniformMat4("view", view);
            modelShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
            modelShader.unbind();

            ourModel->Draw(modelShader);
        }

        //  render skybox
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