#include <iostream>
#include <cstring>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"

const int WIDTH = 800, HEIGHT = 600;
const float pi = 3.14159265358979323846f;

static const char *vShader = "vertexShader.glsl";

static const char *fShader = "fragmentShader.glsl";

std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList;

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

void CreateShaders()
{
    Shader *shader = new Shader();
    shader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader);
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
    CreateShaders();

    float angle = 0;
    GLuint uniformModel = 0;

    while (!glfwWindowShouldClose(mainWindow)) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.5f;
        if(angle >= 360)
            angle = 0;

        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetModelLocation();

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
