#include <glad/glad.h>
#include "Shader.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

Shader::Shader(string vertexPath, string fragmentPath)
{
	GLuint vertexShader = addShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = addShader(fragmentPath, GL_FRAGMENT_SHADER);
	
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	char infoLog[1024];
	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		cerr << "PROGRAM LINKAGE FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertexShader);	// no longer needed after linkage
	glDeleteShader(fragmentShader);
}

GLint Shader::addShader(string shaderPath, GLenum type)
{
	GLuint shader;
	shader = glCreateShader(type);
	string shaderSource = parseShader(shaderPath);
	const GLchar *sSource = shaderSource.c_str();
	glShaderSource(shader, 1, &sSource, NULL);
	glCompileShader(shader);

	// error checking
	char infoLog[1024];
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		string shaderType;
		switch (type)
		{
			case GL_VERTEX_SHADER:
				shaderType = "VERTEX"; break;
			case GL_FRAGMENT_SHADER:
				shaderType = "FRAGMENT"; break;
		}
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		cerr << shaderType << " SHADER COMPILATION FAILED\n" << infoLog << endl;
		return 0;
	}
	return shader;

}

string Shader::parseShader(string path)
{
	ifstream in(path);
	string buffer = [&in] {
		std::ostringstream ss{};
		ss << in.rdbuf();
		return ss.str();
	}();
	in.close();
	return buffer;
}

GLuint Shader::getProgramID() { return ID; }