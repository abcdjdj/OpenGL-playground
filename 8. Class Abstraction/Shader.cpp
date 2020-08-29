#include "Shader.hpp"

Shader::Shader()
{
    shaderID = 0;
    uniformModel = 0;
}

void Shader::CreateFromFiles(const char *vertexFile, const char *fragmentFile)
{
    std::string vShaderSrc = ReadFile(vertexFile);
    std::string fShaderSrc = ReadFile(fragmentFile);
    CreateFromString(vShaderSrc.c_str(), fShaderSrc.c_str());
}

std::string Shader::ReadFile(const char *filePath)
{
    std::string contents;
    std::ifstream fileStream(filePath, std::ios::in);
    
    if(!fileStream.is_open()) {
        std::cout << "Can't read file " << filePath << std::endl;
        return "";
    }
    
    std::string line;
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        contents.append(line);
        contents.append("\n");
    }
    
    return contents;
}

void Shader::CreateFromString(const char *vertexCode, const char *fragmentCode)
{
    CompileShader(vertexCode, fragmentCode);
}

void Shader::AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType)
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

void Shader::CompileShader(const char *vertexCode, const char *fragmentCode)
{
    shaderID = glCreateProgram();
    if(!shaderID) {
        std::cout << "Error creating shader program!" << std::endl;
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar elog[1024] = { 0 };

    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if(!result) {
        glGetProgramInfoLog(shaderID, sizeof(elog), NULL, elog);
        std::cout << "Error linking shader : " << elog << std::endl;
        return;
    }

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if(!result) {
        glGetProgramInfoLog(shaderID, sizeof(elog), NULL, elog);
        std::cout << "Error validating shader : " << elog << std::endl;
        return;
    }

    uniformModel = glGetUniformLocation(shaderID, "model");
}

GLuint Shader::GetModelLocation()
{
    return uniformModel;
}

void Shader::UseShader()
{
    glUseProgram(shaderID);
}

void Shader::ClearShader()
{
    if(shaderID != 0) {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }
    
    uniformModel = 0;
}

Shader::~Shader()
{
    ClearShader();
}
