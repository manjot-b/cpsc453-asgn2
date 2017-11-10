/*
*	This class takes a vertex shader and fragment shader,
*	compiles and then links them into a program.
*/


#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <vector>

class Shader 
{
	public: 
		
		Shader(std::string vertexPath, std::string fragmentPath);
		~Shader();
		GLuint getProgramID();
		/*
			Compiles and links the shaders to a program.
			The program's ID is stored in the ID memeber 
			variable.
		*/
		bool addShader(std::string shaderPath, GLuint type);
		bool link();
	
	private:
		GLuint ID;
		std::vector<GLuint> shaders;
		std::string parseShader(std::string shaderPath);
};
#endif
