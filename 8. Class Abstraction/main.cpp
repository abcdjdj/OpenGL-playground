#include <iostream>
#include <cstring>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"

const int WIDTH = 800, HEIGHT = 600;
const float pi = 3.14159265358979323846f;

static const char *vShader =
"#version 450\n\
layout(location = 0) in vec3 pos;\n\
out vec4 vCol;\n\
uniform mat4 model;\n\
\
void main() {\n\
    gl_Position = model * vec4(pos.x * 0.4, pos.y * 0.4, pos.z, 1.0);\n\
    vCol = vec4(clamp(pos, 0.0, 1.0), 1.0);\n\
}";

static const char *fShader =
"#version 450\n\
out vec4 color;\n\
in vec4 vCol;\n\
\
void main() {\n\
    color = vCol;\n\
}";

GLuint shader, uniformModel;

std::vector<Mesh *> meshList;

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
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        1, 2, 3,
        0, 1, 3
    };

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj1);

    Mesh *obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj2);
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

    glEnable(GL_DEPTH_TEST);

    CreateTriangle();
    CompileShaders();

    float angle = 0;
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.5f;
        if(angle >= 360)
            angle = 0;
        glUseProgram(shader);

        glm::mat4 model{1.0f};
        model = glm::rotate(model, angle*pi/180, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::translate(model, glm::vec3(0, -0.5f, 0));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
       
        meshList[0]->RenderMesh();


        glm::mat4 model2{1.0f};
        model2 = glm::translate(model2, glm::vec3(0, 0.5f, 0));
        model2 = glm::rotate(model2, -angle*pi/180, glm::vec3(0.0f, 1.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model2));

        meshList[1]->RenderMesh();

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }
}