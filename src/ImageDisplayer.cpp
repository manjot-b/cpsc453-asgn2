#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ImageDisplayer.h"

using namespace std;

ImageDisplayer::ImageDisplayer(const char* _fileName)
{
    verticies = {
                //Triangle coords   	// texture coords
                -1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
                1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
                -1.0f, 1.0f, 0.0f,		0.0f, 1.0f		
				};
	
	indicies = {
				0, 1, 2,
				0, 2, 3
				};
	initWindow();
	texture = new Texture(_fileName);
	adjustAspectRatio(); 
	shader = new Shader("rsc/vertex.glsl", "rsc/fragment.glsl");
}

ImageDisplayer::~ImageDisplayer()
{
	delete texture;
	delete shader;
}

int ImageDisplayer::initWindow()
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "Image Displayer", NULL, NULL);

	if (!window)
	{
		cout << "Failed to create GLFW window. TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetWindowAspectRatio(window, 1, 1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, 
		[] (GLFWwindow *window, int width, int height) {
			glViewport(0, 0, width, height);
		}
	);
	
	glGenBuffers(1, &VBO); // gen 1 buffer and store id in VBO
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(float), indicies.data(), GL_STATIC_DRAW);

	const int TEXTURE_COMPONENTS = 2;
	const int VERTEX_COMPONENTS = 3;
	const int COMPONENTS_PER_VERTEX = TEXTURE_COMPONENTS + VERTEX_COMPONENTS;
	const int STRIDE = COMPONENTS_PER_VERTEX * sizeof(float);
	//position attributes
	glVertexAttribPointer(0, VERTEX_COMPONENTS, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
	glEnableVertexAttribArray(0);
	//texture attributes
	glVertexAttribPointer(1, TEXTURE_COMPONENTS, GL_FLOAT, GL_FALSE, STRIDE, (void*)(VERTEX_COMPONENTS * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind VAO, VBO, and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	

	return 0;	// if we made it here then success
}

int ImageDisplayer::run()
{
	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader->getProgramID());
		glBindTexture(GL_TEXTURE_2D, texture->getID());
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void ImageDisplayer::adjustAspectRatio()
{
	float ratio = (float)texture->getWidth() / texture->getHeight();
	
	verticies[0] = -1.0f;
	verticies[1] = -1.0f + (2.0f - 2.0f / ratio) / 2;

	verticies[5] = 1.0f;
	verticies[6] = -1.0f + (2.0f - 2.0f / ratio) / 2;
		
	verticies[10] = 1.0f;
	verticies[11] = 1.0f - (2.0f - 2.0f / ratio) / 2;

	verticies[15] = -1.0f;
	verticies[16] = -1.0f + (2.0f - 2.0f / ratio) / 2;
	
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);
}