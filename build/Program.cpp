#include "Program.h"

#include "ExceptionWrapper.h"
#include "GLSL.h"

using namespace GLSL;
using namespace std;

Program::Program()
{
}

Program::Program(const std::string & vertexShader, const std::string & fragShader)
{
	// Stores error code
	GLint rc = 0;

	// Compiling the vertex shader
	this->vert = compileShader(vertexShader, GL_VERTEX_SHADER, &rc);
	if (!rc) throw_exception(runtime_error("Error compiling vertex shader: " + vertexShader));

	// Compiling the fragment shader
	this->frag = compileShader(fragShader, GL_FRAGMENT_SHADER, &rc);
	if (!rc) throw_exception(runtime_error("Error compiling fragment shader: " + fragShader));

	// Creating the program and linking shaders
	this->prog = glCreateProgram();
	glAttachShader(this->prog, this->vert);
	glAttachShader(this->prog, this->frag);
	glLinkProgram(this->prog);

	// Checking for errors
	printError();
	glGetProgramiv(this->prog, GL_LINK_STATUS, &rc);
	printProgramInfoLog(this->prog);
	if (!rc) throw_exception(runtime_error("Error linking shaders " + vertexShader + " and " + fragShader));

	// Sets up uniform/attribute handles
	setupHandles();
}

Program::~Program()
{
}

GLuint Program::compileShader(const std::string& file, GLenum shaderType, GLint* rc)
{
	GLuint shader = glCreateShader(shaderType);
	const char *contents = textFileRead(file.c_str());
	glShaderSource(shader, 1, &contents, NULL);
	glCompileShader(shader);
	printError();
	glGetShaderiv(shader, GL_COMPILE_STATUS, rc);
	printShaderInfoLog(shader);
	return shader;
}

void Program::setupHandles()
{
	// Adds all uniforms to the uniform map
	int total = -1;
	glGetProgramiv(this->prog, GL_ACTIVE_UNIFORMS, &total);
	for (int i = 0; i<total; ++i)
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(this->prog, GLuint(i), sizeof(name) - 1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetUniformLocation(this->prog, name);
		uniforms.insert(pair<string, GLint>(name, location));
	}

	// Adds all attributes to the uniform map
	total = -1;
	glGetProgramiv(this->prog, GL_ACTIVE_ATTRIBUTES, &total);
	for (int i = 0; i<total; ++i)
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveAttrib(this->prog, GLuint(i), sizeof(name) - 1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetAttribLocation(this->prog, name);
		attributes.insert(pair<string, GLint>(name, location));
	}
}

GLint Program::getAttributeHandle(string name)
{
	GLint handle;
	try
	{
		handle = this->attributes.at(name);
	}
	catch (exception& e)
	{
		throw_exception(runtime_error("The attribute name '" + name + "' is not bound to this program."));
	}
	return handle;
}

GLint Program::getUniformHandle(string name)
{
	GLint handle;
	try
	{
		handle = this->uniforms.at(name);
	}
	catch (exception& e)
	{
		throw_exception(runtime_error("The uniform name '" + name + "' is not bound to this program."));
	}
	return handle;
}