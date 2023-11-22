#include "main.h"
#include "stb_image.h"  

#include <filesystem>

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
   
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); //if z value is less (closer to us), pass the depth test.
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //mask the stencil buffer to all 1's, set default all fragments fail stencil.
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //if failed stencil or depth, keep the value inside stencil buffer. If passed both, replace value with 1

    // create shader
    Shader ourShader = Shader::Shader("vertex.shader", "frag.shader");
    Shader borderShader = Shader::Shader("shaderBorderColorVertex.shader", "shaderBorderColor.shader");
    Shader lightShader = Shader::Shader("vertexLight.shader", "fragLight.shader");

    std::string workingDir = std::filesystem::current_path().generic_string();
    Model ourModel(workingDir + "/" + "resources/model/backpack/backpack.obj");  

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        glUseProgram(ourShader.createShaderProgram());

        // global light
        ourShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        ourShader.addUniform3f("pointLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.addUniform3f("pointLight.diffuse", 0.7f, 0.7f, 0.7f);
        ourShader.addUniform3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.addUniform3f("pointLight.position", 2.0f, 1.0f, 1.0f);

        // view/projection transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.addUniformMat4("projection", projection);
        ourShader.addUniformMat4("view", view);

        // enable stencil writes and set all writes to set 1 in the stencil buffer for the following rendered objects
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); // enable writing to the stencil buffer

        // render the loaded model
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.addUniformMat4("model", model);
        ourModel.Draw(ourShader);

        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.addUniformMat4("model", model);
        ourModel.Draw(ourShader);


        // disable writes, and only pass stencil test where stencil test is NOT 1. 
        // note - since depth test is disabled, the only condition checked is whether the current fragment's stencil buffer value is NOT_EQUAL to 1. If it is not 1, the new fragment is drawn. If it is 1, the new fragment is discarded.
        // we do not need to disable writes to the stencil buffer, even though in the areas where the stencil test passes, the buffer's values are written to the ref value, 1, the buffer is cleared in the next frame.
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);    
        glStencilMask(0x00);

        glUseProgram(borderShader.createShaderProgram());

        model = glm::mat4(1.0f);
        borderShader.addUniformMat4("projection", projection);
        borderShader.addUniformMat4("view", view);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        borderShader.addUniformMat4("model", model);
        ourModel.Draw(borderShader);

        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        borderShader.addUniformMat4("model", model);
        ourModel.Draw(borderShader);

        // re-enable writes, set stencil to always pass and re-enable depth test
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

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