#include "main.h"
#include "stb_image.h"  
#include "cube.h"  

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
    Shader framebufferShader = Shader::Shader("framebuffervertex.shader", "framebufferfrag.shader");
        
    std::string workingDir = std::filesystem::current_path().generic_string();
    Model ourModel(workingDir + "/" + "resources/model/backpack/backpack.obj");  
    
    Cube lightCube = Cube::Cube();

    glUseProgram(framebufferShader.createShaderProgram());
    framebufferShader.addUniform1i("screenTexture", 0);

    // framebufffer
    unsigned int fbo;   
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // attach color reference to framebuffer
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach depth and stencil buffers with rbo
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // detach framebuffer

    // framebuffer quad
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

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
            
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); 
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(lightShader.createShaderProgram());
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glm::vec3 lightMove = glm::vec3(sin(glfwGetTime()), 0, 0);
        glm::vec3 lightPos1 = glm::vec3(cubePositions[0].x + sin(glfwGetTime()) / 4, cubePositions[0].y, cubePositions[0].z);

        lightShader.addUniformMat4("view", view);
        lightShader.addUniformMat4("projection", projection);   
        lightShader.addUniform3f("lightColor", 1.0f, 0.0f, 0.0f);
        
        for (glm::vec3 vec : cubePositions) {   
            model = glm::mat4(1.0f);
            vec += lightMove;

            model = glm::translate(model, vec);
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            lightShader.addUniformMat4("model", model);
            lightCube.Draw(lightShader);
        }

        // don't forget to enable shader before setting uniforms
        glUseProgram(ourShader.createShaderProgram());

        // global light
        ourShader.addUniform3f("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
        ourShader.addUniform3f("pointLight.ambient", 0.1f, 0.1f, 0.1f); 
        ourShader.addUniform3f("pointLight.diffuse", 0.7f, 0.7f, 0.7f);
        ourShader.addUniform3f("pointLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.addUniform3f("pointLight.position", lightPos1.x, lightPos1.y, lightPos1.z);

        // view/projection transformations
        model = glm::mat4(1.0f);
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

        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 3.0f));
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

        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 3.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        borderShader.addUniformMat4("model", model);
        ourModel.Draw(borderShader);

        // re-enable writes, set stencil to always pass and re-enable depth test
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(framebufferShader.createShaderProgram());
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);   
        glDrawArrays(GL_TRIANGLES, 0, 6);

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