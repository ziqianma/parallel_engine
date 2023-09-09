#include "main.h"
#include "shader.h"
#include "texture.h"
        
int main(void)  
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Error" << std::endl;


    std::cout << glGetString(GL_VERSION) << std::endl;

    Shader shaderProgramObj = Shader::Shader("vertex.shader", "frag.shader");
    unsigned int shaderProgram = shaderProgramObj.createShaderProgram();
    shaderProgramObj.addUniform1i("texture1", 0);


    /*
    float vertices[] = {    
             // pos               color         tex-coords
        -0.25f, 0.25f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left (0)
        0.25f, 0.25f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // top-right (1)
        -0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom-left (2)
        0.25f, -0.25f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f  // bottom-right (3)
    };  
    */
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    // Intialize vertex array and vertex buffer object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    TextureLoader loader;
    Texture texture1 = Texture::Texture(0, "dvd.png");

    loader.addTexture(0, texture1);
    loader.loadTexture(0);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float x = 0.0f, y = 0.0f, z = 0.0f;
    float dx = 0.01f, dy = 0.0f, dz = 0.01f;

    glm::mat4 translate = glm::mat4(1.0f);

    /* Loop until the user closes the window */ 
    while (!glfwWindowShouldClose(window))
    {   
        /* Input check */   
            
        /* Render here */
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        loader.bindTextures();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        
        x += dx;
        y += dy;
        z += dz;

        if (x >= 1.0f || x <= -1.0f) dx = -dx;
        if (y >= 1.0f || y <= -1.0f) dy = -dy;
        if (z >= 3.0f || z <= 0.0f) dz = -dz;
        
        std::cout << "position: " << "(x: " << x << ", y: " << y << ", z: " << z << ")" << std::endl;

        translate = glm::translate(translate, glm::vec3(dx, dy, -dz));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        shaderProgramObj.addUniformMat4("model", model);
        shaderProgramObj.addUniformMat4("view", view);
        shaderProgramObj.addUniformMat4("projection", projection);
        shaderProgramObj.addUniformMat4("translate", translate);
        shaderProgramObj.addUniform4f("ourColor", 0.5f + sin(x) / 2, 0.5f + sin(y) / 2, 0.5f + sin(z) / 2, 1.0f);
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}