#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <planets.h>
#include <vboClass.h>
#include <particles.h>

#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <time.h>

#define SUN_RADIUS 9.90f * 0.5f
#define PI 3.14159265f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void glfwSetWindowCenter(GLFWwindow* window);
void printVec(glm::vec3 v, std::string s);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// particles: may cause stack overflow if this line is in main.
# define PNUM_S 105
# define PNUM PNUM_S * PNUM_S
Particle part[PNUM];

char lightMode = 'r';

int main()
{
    srand((unsigned)time(NULL));

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "particles", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowCenter(window);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader sunShader("../shaders/sun_Vshader.glsl", "../shaders/sun_Fshader.glsl");
    Shader moonShader("../shaders/moon_Vshader.glsl", "../shaders/moon_Fshader.glsl");
    Shader particleShader("../shaders/particle_Vshader.glsl", "../shaders/particle_Fshader.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    vboReader globe("../resources/models/globe.vbo");
    float my_triangle[] =
    {
        // positions             // texture coords
          -0.5f, -0.5f, -0.5f,      0.0f,  0.0f,
           0.5f, -0.5f, -0.5f,      1.0f,  0.0f,
           0.5f,  0.5f, -0.5f,      1.0f,  1.0f,
           0.5f,  0.5f, -0.5f,      1.0f,  1.0f,
          -0.5f,  0.5f, -0.5f,      0.0f,  1.0f,
          -0.5f, -0.5f, -0.5f,      0.0f,  0.0f,

          -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,
           0.5f, -0.5f,  0.5f,      1.0f,  0.0f,
           0.5f,  0.5f,  0.5f,       1.0f,  1.0f,
           0.5f,  0.5f,  0.5f,       1.0f,  1.0f,
          -0.5f,  0.5f,  0.5f,      0.0f,  1.0f,
          -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,

          -0.5f,  0.5f,  0.5f,      1.0f,  0.0f,
          -0.5f,  0.5f, -0.5f,      1.0f,  1.0f,
          -0.5f, -0.5f, -0.5f,      0.0f,  1.0f,
          -0.5f, -0.5f, -0.5f,      0.0f,  1.0f,
          -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,
          -0.5f,  0.5f,  0.5f,      1.0f,  0.0f,

           0.5f,  0.5f,  0.5f,       1.0f,  0.0f,
           0.5f,  0.5f, -0.5f,      1.0f,  1.0f,
           0.5f, -0.5f, -0.5f,      0.0f,  1.0f,
           0.5f, -0.5f, -0.5f,      0.0f,  1.0f,
           0.5f, -0.5f,  0.5f,      0.0f,  0.0f,
           0.5f,  0.5f,  0.5f,       1.0f,  0.0f,

          -0.5f, -0.5f, -0.5f,      0.0f,  1.0f,
           0.5f, -0.5f, -0.5f,      1.0f,  1.0f,
           0.5f, -0.5f,  0.5f,      1.0f,  0.0f,
           0.5f, -0.5f,  0.5f,      1.0f,  0.0f,
          -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,
          -0.5f, -0.5f, -0.5f,      0.0f,  1.0f,

          -0.5f,  0.5f, -0.5f,      0.0f,  1.0f,
           0.5f,  0.5f, -0.5f,      1.0f,  1.0f,
           0.5f,  0.5f,  0.5f,       1.0f,  0.0f,
           0.5f,  0.5f,  0.5f,       1.0f,  0.0f,
          -0.5f,  0.5f,  0.5f,      0.0f,  0.0f,
          -0.5f,  0.5f, -0.5f,      0.0f,  1.0f
    };

    // first, configure the VAO and VBO
    unsigned int globeVBO, globeVAO;
    glGenVertexArrays(1, &globeVAO);
    glGenBuffers(1, &globeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, globeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(globe.buf), globe.buf, GL_STATIC_DRAW);

    glBindVertexArray(globeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int triangleVBO, triangleVAO;
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(my_triangle), my_triangle, GL_STATIC_DRAW);

    glBindVertexArray(triangleVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // shader configuration
    // --------------------
    moonShader.use();

    // load texture
    unsigned int textureSun = loadTexture("../resources/textures/sun.jpg");
    unsigned int textureMoon = loadTexture("../resources/textures/moon.jpg");
    unsigned int textureFlame = loadTexture("../resources/textures/FireBall1.png");

    // create a sun and a moon
    star sun = star(100.0f, glm::vec3(0.0f,  0.0f, 0.0f));
    star moon = star(1.0f, glm::vec3(10.0f, -0.0f, 0.0f), glm::vec3(0.0f, -0.0f, 7.5f));

    // particles
    float k1 = 2 * SUN_RADIUS / PNUM_S;
    float k2 = 2 * PI / PNUM_S;
    for (int i = 0; i < PNUM_S; ++i) {
        for (int j = 0; j < PNUM_S; ++j) {
            float rand_k = 0.9f * (rand() % 201 - 100) / 100.0f;
            part[PNUM_S * i + j].z = -SUN_RADIUS + (i + rand_k) * k1;
            part[PNUM_S * i + j].theta = (j + rand_k) * k2;
            part[PNUM_S * i + j].phi = acos(part[PNUM_S * i + j].z / SUN_RADIUS);
            part[PNUM_S * i + j].phase = rand_k;
            part[PNUM_S * i + j].pos = SUN_RADIUS + rand_k;
            part[PNUM_S * i + j].limit = 0.5f + rand_k;
        }
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        moonShader.use();
        moonShader.setVec3("viewPos", camera.Position);
        moonShader.setFloat("material.shininess", 30.0f);
        moonShader.setVec3("material.ambient", 0.04f, 0.04f, 0.04f);
        moonShader.setVec3("material.diffuse", 1.f, 1.f, 1.f);
        moonShader.setVec3("material.specular", .5f, .5f, .5f);
        // point light
        moonShader.setVec3("pointLights[0].position",0.0f, 0.0f, 0.0f);
        moonShader.setVec3("pointLights[1].position", 10.0f, 0.0f, 0.0f);
        moonShader.setVec3("pointLights[2].position", -10.0f, 0.0f, 0.0f);
        moonShader.setVec3("pointLights[3].position", 0.0f, 10.0f, 0.0f);
        moonShader.setVec3("pointLights[4].position", 0.0f, -10.0f, 0.0f);
        moonShader.setVec3("pointLights[5].position", 0.0f, 0.0f, 10.0f);
        moonShader.setVec3("pointLights[6].position", 0.0f, 0.0f, -10.0f);

        for (int i = 0; i < 7; ++i)
        {
            char buf[5];
            memset(buf, 0, sizeof(buf));
            _itoa_s(i, buf, 10);
            moonShader.setVec3("pointLights[" + std::string(buf) + "].color", 1.0f, 0.5f, 0.0f);
            moonShader.setFloat("pointLights[" + std::string(buf) + "].constant", 1.0f);
            moonShader.setFloat("pointLights[" + std::string(buf) + "].linear", 0.02f);
            moonShader.setFloat("pointLights[" + std::string(buf) + "].quadratic", 0.01f);
        }

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 120.0f);
        glm::mat4 view = camera.GetViewMatrix();
        moonShader.setMat4("projection", projection);
        moonShader.setMat4("view", view);

        sunShader.use();
        
        // render sun
        glBindVertexArray(globeVAO);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureSun);

        // lightMode
        if (lightMode == 'r')
            sunShader.setInt("lightMode", 0);
        else if (lightMode == 'g')
            sunShader.setInt("lightMode", 1);
        else if (lightMode == 'b')
            sunShader.setInt("lightMode", 2);
        else if (lightMode == '4')
            sunShader.setInt("lightMode", 3);
        else if (lightMode == '5')
            sunShader.setInt("lightMode", 4);
        else if (lightMode == '6')
            sunShader.setInt("lightMode", 5);

        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 sunModel = glm::mat4(1.0f);
        sunModel = glm::translate(sunModel, sun.pos);
        float angle = 2.0 * currentFrame;
        sunModel = glm::rotate(sunModel, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        sunModel = glm::scale(sunModel, glm::vec3(0.5f));
        

        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);
        sunShader.setMat4("model", sunModel);

        glDrawArrays(GL_TRIANGLES, 0, 3 * globe.vertexNum);

        moonShader.use();

        // render moon
        glBindVertexArray(globeVAO);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureMoon);

        // lightMode
        if (lightMode == 'r')
            moonShader.setInt("lightMode", 0);
        else if (lightMode == 'g')
            moonShader.setInt("lightMode", 1);
        else if (lightMode == 'b')
            moonShader.setInt("lightMode", 2);
        else if (lightMode == '4')
            moonShader.setInt("lightMode", 3);
        else if (lightMode == '5')
            moonShader.setInt("lightMode", 4);
        else if (lightMode == '6')
            moonShader.setInt("lightMode", 5);

        // physical simulation
        float dis2 = moon.pos.x * moon.pos.x + moon.pos.y * moon.pos.y + moon.pos.z * moon.pos.z;
        glm::vec3 a = 4.0f * glm::normalize(glm::vec3(0.0f) - moon.pos) * sun.mass * moon.mass / dis2;
        moon.vel += a * deltaTime;
        moon.pos += moon.vel * deltaTime;

        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 moonModel = glm::mat4(1.0f);
        moonModel = glm::translate(moonModel, moon.pos);
        moonModel = glm::rotate(moonModel, glm::radians(50*currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
        moonModel = glm::scale(moonModel, glm::vec3(0.08f));


        moonShader.setMat4("projection", projection);
        moonShader.setMat4("view", view);
        moonShader.setMat4("model", moonModel);

        glDrawArrays(GL_TRIANGLES, 0, 3 * globe.vertexNum);


        // render particles

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBindVertexArray(triangleVAO);
        
        particleShader.use();

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureFlame);

        // lightMode
        if (lightMode == 'r')
            particleShader.setInt("lightMode", 0);
        else if (lightMode == 'g')
            particleShader.setInt("lightMode", 1);
        else if (lightMode == 'b')
            particleShader.setInt("lightMode", 2);
        else if (lightMode == '4')
            particleShader.setInt("lightMode", 3);
        else if (lightMode == '5')
            particleShader.setInt("lightMode", 4);
        else if (lightMode == '6')
            particleShader.setInt("lightMode", 5);

        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);

        for (int i = 0; i < PNUM; ++i)
        {
            part[i].pos += 2 * deltaTime ;
            if (part[i].pos > 1.0f + 0.7 * part[i].limit) {
                part[i].pos -= 2.0f;
                part[i].limit = 1.1f * exp((rand() % 100 )/100.0f);
            }
            float part_radius = SUN_RADIUS  + 0.25 * part[i].pos;
            float attenuation = 1.0f + 3 * (part_radius - SUN_RADIUS);
            glm::mat4 particleModel = glm::mat4(1.0f);
            float rho = sqrt(part_radius * part_radius - part[i].z * part[i].z);
            particleModel = glm::translate(particleModel, glm::vec3(rho * glm::cos(part[i].theta), rho * glm::sin(part[i].theta), part[i].z));
            /*if (part[i].theta < PI)
                particleModel = glm::rotate(particleModel, -part[i].phi + PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
            else
                particleModel = glm::rotate(particleModel, -part[i].phi + PI / 2, glm::vec3(-1.0f, 0.0f, 0.0f));
            particleModel = glm::rotate(particleModel, part[i].theta - PI/2, glm::vec3(0.0f, 0.0f, 1.0f));*/
            float mix_k = (1 / attenuation) > 1.0f ? 1.0f : (1 / attenuation);
            particleModel = glm::scale(particleModel, glm::vec3(0.4f/ attenuation));
            particleShader.setMat4("model", particleModel);
            particleShader.setFloat("alpha", 0.18 / exp(0.2 * attenuation));
            particleShader.setFloat("mix_k", mix_k);
            glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
        }

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // ------------------------------------------------------------------------------- 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &globeVAO);
    glDeleteBuffers(1, &globeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        lightMode = 'r';
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        lightMode = 'g';
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        lightMode = 'b';
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        lightMode = '4';
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        lightMode = '5';
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        lightMode = '6';
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// set the window in the center
// --------------------------------
void glfwSetWindowCenter(GLFWwindow* window) {
    // Get window position and size
    int window_x, window_y;
    glfwGetWindowPos(window, &window_x, &window_y);
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    // Halve the window size and use it to adjust the window position to the center of the window
    window_width *= 0.5;
    window_height *= 0.5;
    window_x += window_width;
    window_y += window_height;
    // Get the list of monitors
    int monitors_length;
    GLFWmonitor** monitors = glfwGetMonitors(&monitors_length);
    if (monitors == NULL) {
        // Got no monitors back
        return;
    }
    // Figure out which monitor the window is in
    GLFWmonitor* owner = NULL;
    int owner_x, owner_y, owner_width, owner_height;
    for (int i = 0; i < monitors_length; i++) {
        // Get the monitor position
        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);
        // Get the monitor size from its video mode
        int monitor_width, monitor_height;
        GLFWvidmode* monitor_vidmode = (GLFWvidmode*)glfwGetVideoMode(monitors[i]);
        if (monitor_vidmode == NULL) {
            // Video mode is required for width and height, so skip this monitor
            continue;
        }
        else {
            monitor_width = monitor_vidmode->width;
            monitor_height = monitor_vidmode->height;
        }
        // Set the owner to this monitor if the center of the window is within its bounding box
        if ((window_x > monitor_x && window_x < (monitor_x + monitor_width)) && (window_y > monitor_y && window_y < (monitor_y + monitor_height))) {
            owner = monitors[i];
            owner_x = monitor_x;
            owner_y = monitor_y;
            owner_width = monitor_width;
            owner_height = monitor_height;
        }
    }
    if (owner != NULL) {
        // Set the window position to the center of the owner monitor
        glfwSetWindowPos(window, owner_x + (owner_width * 0.5) - window_width, owner_y + (owner_height * 0.5) - window_height);
    }
}


void printVec(glm::vec3 v, std::string s="")
{
    std::cout << s <<" : " << v.x << " " << v.y << " " << v.z << " " << std::endl;
}