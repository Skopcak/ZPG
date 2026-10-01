#include "ShaderProgram.h"
#include "Shader.h"

#include <cstdlib>
#include <iostream>

ShaderProgram::ShaderProgram(const char* vertexFile, const char* fragmentFile)
    : programID(0)
{
    // Konstruktory Shader nacitaju subory a skompiluju ich.
    Shader vertexShader(GL_VERTEX_SHADER, vertexFile);
    Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentFile);

    programID = glCreateProgram();
    if (programID == 0)
    {
        std::cerr << "Unable to create shader program" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glAttachShader(programID, vertexShader.getID());
    glAttachShader(programID, fragmentShader.getID());
    glLinkProgram(programID);

    GLint success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program link failed: " << vertexFile << " + "
                  << fragmentFile << '\n' << infoLog << std::endl;
        glDeleteProgram(programID);
        std::exit(EXIT_FAILURE);
    }

    // Po uspesnom linkovani program funguje aj bez samostatnych shaderov.
    glDetachShader(programID, vertexShader.getID());
    glDetachShader(programID, fragmentShader.getID());
    // Pri odchode z konstruktora sa lokalne objekty Shader automaticky zrusia.
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programID);
}

void ShaderProgram::use() const
{
    glUseProgram(programID);
}


//glUniform3 pouziva na posun 
void ShaderProgram::setUniform(
    const char* name, float x, float y, float z) const
{
    GLint location = glGetUniformLocation(programID, name);

    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << std::endl;
        std::exit(EXIT_FAILURE);
    }

    use();
    glUniform3f(location, x, y, z);
}

void ShaderProgram::setUniform(
    const char* name, float value) const
{
    GLint location = glGetUniformLocation(programID, name);

    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << '\n';
        std::exit(EXIT_FAILURE);
    }

    use();
    glUniform1f(location, value);
}