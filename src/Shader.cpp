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
	ID = glCreateProgram();
	
	addShader(vertexPath, GL_VERTEX_SHADER);
	addShader(fragmentPath, GL_FRAGMENT_SHADER);
	
	//glAttachShader(ID, vertexShader);
	//glAttachShader(ID, fragmentShader);
	
}

Shader::~Shader()
{
	for (auto const &shader : shaders)
	{
		glDeleteShader(shader);
	}
}

bool Shader::addShader(string shaderPath, GLenum type)
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
			case GL_TESS_CONTROL_SHADER:
				shaderType = "TESS CONTROL"; break;
			case GL_TESS_EVALUATION_SHADER:
				shaderType = "TESS EVALUATION"; break;
		}
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		cerr << shaderType << " SHADER COMPILATION FAILED\n" << 
			shaderPath << endl << infoLog << endl;
		return 0;
	}
	glAttachShader(ID, shader);
	return success;

}

bool Shader::link()
{
	glLinkProgram(ID);
	
	char infoLog[1024];
	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		cerr << "PROGRAM LINKAGE FAILED\n" << infoLog << endl;
		
	}
	return success;
	//glDeleteShader(vertexShader);	// no longer needed after linkage
	//glDeleteShader(fragmentShader);
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