#ifndef READSHADER_HPP
#define READSHADER_HPP

#include <string>
#include <GL\glew.h>

std::string readShader(const char *filePath);

bool CreateProgram(GLuint &programID, std::string shadernames[], GLenum shaderType[], int numberOfShaders);

#endif