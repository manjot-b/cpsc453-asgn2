/*
*	This class takes a vertex shader and fragment shader,
*	compiles and then links them into a program.
*/


#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

class Shader 
{
	public: 
		
		Shader(std::string vertexPath, std::string fragmentPath);
		GLuint getProgramID();
	
	private:
		GLuint ID;
		/*
			Compiles and links the shaders to a program.
			The program's ID is stored in the ID memeber 
			variable.
		*/
		GLint addShader(std::string shaderPath, GLuint type);
		std::string parseShader(std::string shaderPath);
};
#endif
