
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <initializer_list>

#include <sphere.h>

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
    printf("resize %d, %d \n", width, height);
    glViewport(0, 0, width, height);
}

// Funkce pro načtení a kompilaci shaderu ze souboru
GLuint createShaderFromFile(GLenum shaderType, const char* shaderFile) {
    GLuint shaderID = glCreateShader(shaderType);

    if (shaderID == 0) {
        std::cout << "Unable to create shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream file(shaderFile);
    if (!file.is_open()) {
        std::cout << "Unable to open file " << shaderFile << std::endl;
        glDeleteShader(shaderID);
        exit(-1);
    }

    std::string shaderCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const char* source = shaderCode.c_str();

    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog);
        std::cout << "Shader failed:\n" << infoLog << std::endl;
        glDeleteShader(shaderID);
        exit(1);
    }
    return shaderID;
}


int main(void) {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "ZPG", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Inicializace GLAD2
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("GLAD initialization failed\n");
        return -1;
    }

    // Výpis verzí
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, window_size_callback);

    // Úprava modelu štvorce (2 trojúhelníky = 6 vrcholů): XYZ Pozice (3x float) + RGB Barva (3x float)
    //float points[] = {
    //    // Poloha: x, y, z        Farba: r, g, b
    //     0.0f,  0.5f, 0.0f,      1.0f, 0.0f, 0.0f, // Horný – červený
    //     0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f, // Pravý dolný – zelený
    //    -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f  // Ľavý dolný – modrý
    //};

    float points[] = {
        // 1. Trojúhelník
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Červená
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // Zelená
         0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // Modrá

         // 2. Trojúhelník
         -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Červená
          0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // Modrá
         -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f  // Žlutá
    };

    // Vytvoření VBO
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);

    // Vytvoření VAO
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0); // Pozice
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);

    glEnableVertexAttribArray(1); // Atribut 1: druha trojica hodnot, vstup color vo vertex shaderi
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

    // Nacitanie a kompilacia shaderov
    GLuint vertexShader = createShaderFromFile(
        GL_VERTEX_SHADER, "shaders/basic.vert");
    GLuint yellowVertexShader = createShaderFromFile(
        GL_VERTEX_SHADER, "shaders/yellow.vert");
    GLuint fragmentShader = createShaderFromFile(
        GL_FRAGMENT_SHADER, "shaders/basic.frag");
    GLuint yellowFragmentShader = createShaderFromFile(
        GL_FRAGMENT_SHADER, "shaders/yellow.frag");

    // Program pre farebnu gulu/
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Program pre zltu gulu: vlastny vertex aj fragment shader
    GLuint yellowProgram = glCreateProgram();
    glAttachShader(yellowProgram, yellowVertexShader);
    glAttachShader(yellowProgram, yellowFragmentShader);
    glLinkProgram(yellowProgram);

    // Overenie linkovania oboch programov
    for (GLuint program : {shaderProgram, yellowProgram})
    {
        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            std::cout << "Program link failed:\n" << infoLog << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(yellowVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(yellowFragmentShader);

    // Jeden vrchol obsahuje 3 suradnice a 3 zlozky normaly
    const GLsizei sphereVertexCount = static_cast<GLsizei>(
        sizeof(sphere) / (6 * sizeof(float)));

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Zachovanie trojuholnikoveho vzoru z experimentu v bode 4.
    // Pre spravne zakryvanie povrchov pouzi glEnable(GL_DEPTH_TEST).
    glDisable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);

        // Farebna gula vlavo: posun je zapisany v basic.vert
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);

        // Zlta gula vpravo: posun je zapisany v yellow.vert
        glUseProgram(yellowProgram);
        glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glUseProgram(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(yellowProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
