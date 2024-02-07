#include "main.h"
#include <stb_image.h>

inline std::unique_ptr<Shader> modelShader;
inline std::unique_ptr<Shader> lightCubeShader;
inline std::unique_ptr<Shader> cubeShader;
inline std::unique_ptr<Shader> skyboxShader;
inline std::unique_ptr<Shader> planeShader;
inline std::unique_ptr<Shader> simpleDepthShader;
inline std::unique_ptr<Shader> depthQuadShader;

inline std::unique_ptr<Skybox> skybox;
inline std::unique_ptr<Model> ourModel;
inline std::unique_ptr<Cube> lightCube;
inline std::unique_ptr<Cube> cubeGroup;
inline std::unique_ptr<Plane> floorPlane;

inline std::unique_ptr<FrameBuffer> depth_fb;

inline void DrawScene();
inline void DrawSceneSimpleDepth(const glm::mat4& lightSpaceMatrix);
inline void SetupShaderUniforms(const glm::mat4& lightSpaceMatrix);

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
    modelShader = std::make_unique<Shader>("shaders/main.vs", "shaders/shadow.fs");

    lightCubeShader = std::make_unique<Shader>("shaders/cube.vs", "shaders/unlit.fs");
    cubeShader = std::make_unique<Shader>("shaders/cube.vs", "shaders/shadow.fs");

    skyboxShader = std::make_unique<Shader>("shaders/skybox.vs", "shaders/skybox.fs");
    planeShader = std::make_unique<Shader>("shaders/plane.vs", "shaders/shadow.fs");

    simpleDepthShader = std::make_unique<Shader>("shaders/simple_depthMap.vs", "shaders/simple_depthMap.fs");

    depthQuadShader = std::make_unique<Shader>("shaders/quad.vs", "shaders/depth.fs");
    depth_fb = std::make_unique<FrameBuffer>(*depthQuadShader,
        nullptr, "screenTexture",
        game_constants::SHADOW_WIDTH, game_constants::SHADOW_HEIGHT,
        GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
        game_constants::QUAD2_VERTICES);

    std::vector<glm::mat4> lightCube_ModelMatrices;
    std::vector<Shader> litShaders = { *modelShader, *planeShader, *cubeShader };

    LightGroup lightGroup(litShaders,
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

    std::vector<glm::mat4> cube_ModelMatrices;
    generatePositions(30, cube_ModelMatrices);
      
    // Create skybox
    skybox = std::make_unique<Skybox>(*skyboxShader);

    // Create other game objs
    ourModel = std::make_unique<Model>(WORKING_DIR + "/" + "resources/model/dragon/dragon.obj", *modelShader, model_ModelMatrices, depth_fb->getTextureID());
    lightCube = std::make_unique<Cube>(*lightCubeShader, WORKING_DIR + "/" + "resources/redstone_lamp_on.png", lightCube_ModelMatrices);

    cubeGroup = std::make_unique<Cube>(*cubeShader, WORKING_DIR + "/" + "resources/gold_block.png", cube_ModelMatrices, depth_fb->getTextureID());
    floorPlane = std::make_unique<Plane>(*planeShader, WORKING_DIR + "/" + "resources/floor2.jpg", depth_fb->getTextureID());

    // timing
    float dt = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    int frameCount = 0;

    float angle = 0;

    // light space matrix
    glm::mat4 lightProjection = glm::ortho(
        game_constants::SHADOW_BOUNDING_BOX.x, game_constants::SHADOW_BOUNDING_BOX.y, game_constants::SHADOW_BOUNDING_BOX.z, game_constants::SHADOW_BOUNDING_BOX.w,
        game_constants::NEAR_PLANE_SHADOW, game_constants::FAR_PLANE_SHADOW);

    glm::mat4 lightView = glm::lookAt(
        5.0f * SUN_LIGHT_DIR,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glm::vec3 point_light_diffuse = POINT_LIGHT_DATA.diffuse;

    bool debug = true;
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
            ourModel.reset();
            ourModel = nullptr;
        }


        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            debug = !debug;
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
                lightGroup.update_light_data(litShaders);
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
                lightGroup.update_light_data(litShaders);
            }
        }
       
        SetupShaderUniforms(lightSpaceMatrix);

        // render
        // ------   
        
        TextureLoader::Update();

        // first pass, depth map from light's perspective
        glViewport(0, 0, game_constants::SHADOW_WIDTH, game_constants::SHADOW_HEIGHT);
        depth_fb->bind();
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        DrawSceneSimpleDepth(lightSpaceMatrix);
        glCullFace(GL_BACK);
        depth_fb->unbind();

        glViewport(0, 0, game_constants::SCR_WIDTH, game_constants::SCR_HEIGHT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawScene();

        if (debug) {
            // final pass, draw framebuffer quad
            glDisable(GL_DEPTH_TEST);
            depthQuadShader->bind();
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            depth_fb->draw_onto_quad();
            depthQuadShader->unbind();
        }

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

    float radius = 7.5f;
    float offset = 3.0f;
    float height_offset = 2.0f;

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
        float y = height_offset + displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;

        // add translation portion to return value
        positions.emplace_back(glm::vec3(x, y, z));

        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.5 and 1.0f
        float scale = static_cast<float>((rand() % 100) + 50) / 100.0f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices.push_back(model);


    }

    return positions;
}

void DrawScene() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    //render lights
    lightCube->Draw(*lightCubeShader);

    // render cubes
    cubeGroup->Draw(*cubeShader);

    // render floor
    floorPlane->Draw(*planeShader);

    // render model
    if (ourModel) {
        modelShader->bind();
        glm::mat4 model = glm::mat4(1.0f);
        modelShader->addUniformMat4("model", model);
        modelShader->unbind();

        ourModel->Draw(*modelShader);
    }

    skybox->Draw(*skyboxShader, camera.GetViewMatrix());

}

void DrawSceneSimpleDepth(const glm::mat4& lightSpaceMatrix) {

    simpleDepthShader->bind();
    simpleDepthShader->addUniformMat4("lightSpaceMatrix", lightSpaceMatrix);

    glm::mat4 model = glm::mat4(1.0f);
    simpleDepthShader->addUniformMat4("model", model);
    simpleDepthShader->unbind();

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // render cubes
    cubeGroup->Draw(*simpleDepthShader);

    // render floor
    floorPlane->Draw(*simpleDepthShader);

    // render model
    if (ourModel) {
        ourModel->Draw(*simpleDepthShader);
    }

    skybox->Draw(*skyboxShader, camera.GetViewMatrix());
}

void SetupShaderUniforms(const glm::mat4& lightSpaceMatrix) {
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)game_constants::SCR_WIDTH / (float)game_constants::SCR_HEIGHT, game_constants::NEAR_PLANE, game_constants::FAR_PLANE);
    glm::mat4 view = camera.GetViewMatrix();


    skyboxShader->bind();
    skyboxShader->addUniformMat4("projection", projection);
    skyboxShader->addUniformMat4("view", view);
    skyboxShader->unbind();

    lightCubeShader->bind();
    lightCubeShader->addUniformMat4("projection", projection);
    lightCubeShader->addUniformMat4("view", view);
    lightCubeShader->unbind();

    cubeShader->bind();
    cubeShader->addUniformMat4("projection", projection);
    cubeShader->addUniformMat4("view", view);
    cubeShader->addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
    cubeShader->addUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
    cubeShader->unbind();

    planeShader->bind();
    planeShader->addUniformMat4("projection", projection);
    planeShader->addUniformMat4("view", view);
    planeShader->addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
    planeShader->addUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
    planeShader->unbind();

    modelShader->bind();
    modelShader->addUniformMat4("projection", projection);
    modelShader->addUniformMat4("view", view);
    modelShader->addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
    modelShader->addUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
    modelShader->unbind();
}