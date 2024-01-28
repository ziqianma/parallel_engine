#include "main.h"
#include <stb_image.h>

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    srand(glfwGetTime()); // initialize random seed	

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(game_constants::SCR_WIDTH, game_constants::SCR_HEIGHT, "Learn", NULL, NULL);

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

    Shader lightCubeShader = Shader::Shader("shaders/vertexCube.shader", "shaders/fragUnlit.shader");
    Shader cubeShader = Shader::Shader("shaders/vertexCube.shader", "shaders/frag.shader");

    Shader skyboxShader = Shader::Shader("shaders/skyboxVertex.shader", "shaders/skyboxFrag.shader");
    Shader planeShader = Shader::Shader("shaders/vertexPlane.shader", "shaders/frag.shader");

    std::vector<glm::mat4> lightCube_ModelMatrices;

    LightGroup lightGroup(std::vector<Shader>({ modelShader, planeShader, cubeShader }),
        POINT_LIGHT_DATA,
        generatePositions(NUM_POINT_LIGHTS, lightCube_ModelMatrices),
        SUN_LIGHT_DATA, 
        SUN_LIGHT_DIR);

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

    std::vector<glm::mat4> cube_ModelMatrices;
    for (int z = 0; z < 10; z++) {
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x-15.0f, y-5.0f, z-15.0f));
                model = glm::scale(model, glm::vec3(1.0f));

                cube_ModelMatrices.push_back(model);
            }
        }
    }

    // Create skybox
    Skybox skybox(skyboxShader);

    // Create other game objs
    Model* ourModel = new Model(WORKING_DIR + "/" + "resources/model/backpack/backpack.obj", modelShader, model_ModelMatrices);
    Cube lightCube(lightCubeShader, WORKING_DIR + "/" + "resources/redstone_lamp_on.png", lightCube_ModelMatrices);

    Cube cubeGroup(cubeShader, WORKING_DIR + "/" + "resources/gold_block.png", cube_ModelMatrices);

    Plane floor(planeShader, WORKING_DIR + "/" + "resources/floor2.jpg", floor_ModelMatrix);

    // timing
    float dt = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    int frameCount = 0;

    float angle = 0;

    glm::vec3 point_light_diffuse = POINT_LIGHT_DATA.diffuse;
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

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (glm::any(glm::greaterThan(point_light_diffuse, glm::vec3(0.0f)))) {
                point_light_diffuse -= dt * glm::vec3(0.1f);

                if (point_light_diffuse.x < 0.0f) point_light_diffuse.x = 0.0f;
                if (point_light_diffuse.y < 0.0f) point_light_diffuse.y = 0.0f;
                if (point_light_diffuse.z < 0.0f) point_light_diffuse.z = 0.0f;

                for (int i = 1; i < NUM_POINT_LIGHTS + 1; i++) {
                    lightGroup[i].set_diffuse(point_light_diffuse);
                }
                lightGroup.update_light_data();
            }
        }        
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (glm::any(glm::lessThan(point_light_diffuse, glm::vec3(1.0f)))) {
                point_light_diffuse += dt * glm::vec3(0.1f);

                if (point_light_diffuse.x >= 1.0f) point_light_diffuse.x = 1.0f;
                if (point_light_diffuse.y >= 1.0f) point_light_diffuse.y = 1.0f;
                if (point_light_diffuse.z >= 1.0f) point_light_diffuse.z = 1.0f;

                for (int i = 1; i < NUM_POINT_LIGHTS + 1; i++) {
                    lightGroup[i].set_diffuse(point_light_diffuse);
                }
                lightGroup.update_light_data();
            }
        }
        

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)game_constants::SCR_WIDTH / (float)game_constants::SCR_HEIGHT, 0.1f, 100.0f);

        skyboxShader.bind();
        skyboxShader.addUniformMat4("projection", projection);
        skyboxShader.unbind();
                
        lightCubeShader.bind();
        lightCubeShader.addUniformMat4("projection", projection);
        lightCubeShader.unbind();

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
        //glEnable(GL_CULL_FACE);

        //render lights
        lightCubeShader.bind();
        lightCubeShader.addUniformMat4("view", view);
        lightCubeShader.unbind();

        lightCube.Draw(lightCubeShader);


        // render cubes
        cubeShader.bind();
        cubeShader.addUniformMat4("view", view);
        cubeShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        cubeShader.unbind();

        cubeGroup.Draw(cubeShader);

        // render floor
        planeShader.bind();
        planeShader.addUniformMat4("view", view);
        planeShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        planeShader.unbind();

        floor.Draw(planeShader);

        // render model
        if (ourModel) {
            modelShader.bind();
            glm::mat4 model = glm::mat4(1.0f);
            angle += 0.1*dt;
            model = glm::rotate(model, angle, glm::vec3 (0.0f, 1.0f, 0.0f));
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

std::vector<glm::vec3> generatePositions(int numPointLights, std::vector<glm::mat4>& modelMatrices)
{

    float radius = 5.0f;
    float offset = 2.5f;

    std::vector<glm::vec3> positions;
    if (numPointLights != 0) positions.reserve(numPointLights);

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

        // add translation portion to return value
        positions.emplace_back(glm::vec3(x, y, z));

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

    return positions;
}