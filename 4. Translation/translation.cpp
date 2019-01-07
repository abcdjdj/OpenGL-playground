#include <iostream>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


const int WIDTH = 800, HEIGHT = 600;

static const char *vShader =
"#version 450\n\
layout(location = 0) in vec3 pos;\n\
uniform mat4 model;\n\
\
void main() {\n\
    gl_Position = model * vec4(0.4 * pos.x, pos.y * 0.4, pos.z, 1.0);\n\
}";

static const char *fShader =
"#version 450\n\
out vec4 color;\n\
\
void main() {\n\
    color = vec4(1.0, 0.0, 1.0, 1.0);\n\
}";

GLuint VAO, VBO, shader, uniformModel;

enum TranslationState {
    STATE_GOING_RIGHT,
    STATE_GOING_LEFT
};

void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar **theCode = &shaderCode;
    const GLint codeLength = strlen(shaderCode);
    glShaderSource(theShader, 1, theCode, &codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar elog[1024] = { 0 };
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if(!result) {
        glGetShaderInfoLog(theShader, sizeof(elog), NULL, elog);
        std::cout << "Error compiling" << shaderType << "shader : " << elog << std::endl;
        return;
    }

    glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
    shader = glCreateProgram();
    if(!shader) {
        std::cout << "Error creating shader program!" << std::endl;
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar elog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if(!result) {
        glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
        std::cout << "Error linking shader : " << elog << std::endl;
        return;
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if(!result) {
        glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
        std::cout << "Error validating shader : " << elog << std::endl;
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");
}

void CreateTriangle()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Clear everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(void)
{
    if(glfwInit() != GLFW_TRUE) {
        std::cout << "GLFW Init failed!!" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    // Core Profile = No Backwards Compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "My Triangle", NULL, NULL);
    if (!mainWindow) {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialisation failed!" << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    CreateTriangle();
    CompileShaders();
    
    enum TranslationState state = STATE_GOING_RIGHT;
    float xOffset = 0;
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(state == STATE_GOING_RIGHT)
            xOffset += 0.005f;
        else
            xOffset -= 0.005f;

        if(abs(xOffset) >= 0.7f) {
            if(state == STATE_GOING_LEFT)
                state = STATE_GOING_RIGHT;
            else
                state = STATE_GOING_LEFT;
        }

        glUseProgram(shader);

        glm::mat4 model{1.0f};
        model = glm::translate(model, glm::vec3(xOffset, xOffset, 0.0f));
        // Moves the triangle diagonally

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        
            glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }
}