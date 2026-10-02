#include "Shader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

Shader::Shader(GLenum shaderType, const char* shaderFile)
    : shaderID(0)
{
    std::ifstream file(shaderFile);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file " << shaderFile << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string shaderCode((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

    shaderID = glCreateShader(shaderType);
    if (shaderID == 0)
    {
        std::cerr << "Unable to create shader: " << shaderFile << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const char* source = shaderCode.c_str();
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    GLint success = GL_FALSE;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader failed: " << shaderFile << '\n'
                  << infoLog << std::endl;
        glDeleteShader(shaderID);
        std::exit(EXIT_FAILURE);
    }
}

Shader::~Shader()
{
    glDeleteShader(shaderID);
}

GLuint Shader::getID() const
{
    return shaderID;
}
