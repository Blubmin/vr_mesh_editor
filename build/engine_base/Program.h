#pragma once

#include <map>
#include <string>

#include <GL\glew.h>

class Program
{
public:
    Program();
    Program(const std::string& vertexShader, const std::string& fragShader);
    ~Program();

    GLint getAttributeHandle(std::string name);
    GLint getUniformHandle(std::string name);

    GLuint prog;
    GLuint vert;
    GLuint frag;
    std::map<std::string, GLuint> attributes;
    std::map<std::string, GLuint> uniforms;

private:
    GLuint compileShader(const std::string & file, GLenum shaderType, GLint* rc);
    void setupHandles();
};

