#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:
    Shader();
    ~Shader();
    
    void CreateFromString(const char *vertexCode, const char *fragmentCode);
    void CreateFromFiles(const char *vertexFile, const char *fragmentFile);
    
    GLuint GetModelLocation();
    
    void UseShader();
    void ClearShader();
    
private:
    GLuint shaderID, uniformModel;
    
    void CompileShader(const char *vertexCode, const char *fragmentCode);
    void AddShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
    std::string ReadFile(const char *filePath);
};
