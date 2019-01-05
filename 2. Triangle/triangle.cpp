#include <iostream>
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int WIDTH = 800, HEIGHT = 600;

static const char *vShader =
"#version 450\n\
layout(location = 0) in vec3 pos;\n\
\
void main() {\n\
	gl_Position = vec4(pos, 1.0);\n\
}";

static const char *fShader =
"#version 450\n\
out vec4 color;\n\
\
void main() {\n\
	color = vec4(1.0, 0.0, 1.0, 1.0);\n\
}";

GLuint VAO, VBO, shader;

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
	
    while (!glfwWindowShouldClose(mainWindow)) {
		// Get + Handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);
		
			glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}
}