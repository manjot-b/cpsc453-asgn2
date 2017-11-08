#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ImageDisplayer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

ImageDisplayer::ImageDisplayer(const char* _fileName) : 
	modelView(1.0f), scale(1.0f), prevMouseX(0), prevMouseY(0), mouseX(0), mouseY(0),
	mouseInWindow(false), leftMouseButtonHeld(false), rightMouseButtonClicked(false), 
	holdingImage(false), grayScale(false), twoBitQuant(false), c_KeyHeld(false), v_KeyHeld(false)
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
	catPoints = {0, 0, 0};
	initWindow();
	texture = new Texture(_fileName);
	adjustAspectRatio(); 
	imgShader = new Shader("rsc/vertex.glsl", "rsc/fragment.glsl");
	catmullShader = new Shader("rsc/vertex2.glsl", "rsc/fragment2.glsl");
}

ImageDisplayer::~ImageDisplayer()
{
	delete texture;
	delete imgShader;
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
	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, 
		[] (GLFWwindow *window, int width, int height) {
			glViewport(0, 0, width, height);
		}
	);

	glfwSetCursorEnterCallback(window, 
		[] (GLFWwindow *window, int entered) {
			if (entered)
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseInWindow = true;
			else
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseInWindow = true;
		}
	);

	glfwSetMouseButtonCallback(window,
		[] (GLFWwindow *window, int button, int action, int mods) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->leftMouseButtonHeld = true;
			else 
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->leftMouseButtonHeld = false;

			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->rightMouseButtonClicked = true;
		}
	);

	/*glfwSetCursorPosCallback(window, 
		[] (GLFWwindow *window, double xpos, double ypos) {
			//cout << "X: " << xpos << " Y: " << ypos << endl;
			static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->prevMouseX = 
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseX;
			
			static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->prevMouseY = 
				static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseY;
			
			static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseX = xpos;
			
			static_cast<ImageDisplayer *>(glfwGetWindowUserPointer(window))->mouseY = ypos;
		}
	);*/
	
	glGenBuffers(2, VBO); // gen 1 buffer and store id in VBO
	glGenBuffers(1, &EBO);
	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
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

	// CATMULL ROM POINTS
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBufferData(GL_ARRAY_BUFFER, catPoints.size() * sizeof(float), catPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, VERTEX_COMPONENTS, GL_FLOAT, GL_FALSE, VERTEX_COMPONENTS * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
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
		processInput(window);

		glClearColor(0.3f, 1.0f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(imgShader->getProgramID());
		
		GLint modelViewLocation = glGetUniformLocation(imgShader->getProgramID(), "modelView");
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(modelView));

		GLint grayScaleLocation = glGetUniformLocation(imgShader->getProgramID(), "grayScale");
		glUniform1i(grayScaleLocation, grayScale);

		GLint twoBitQuantLocation = glGetUniformLocation(imgShader->getProgramID(), "twoBitQuant");
		glUniform1i(twoBitQuantLocation, twoBitQuant);

		GLint scaleLocation = glGetUniformLocation(imgShader->getProgramID(), "scale");
		glUniform1f(scaleLocation, scale);
		
		glBindTexture(GL_TEXTURE_2D, texture->getID());
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
		
		//CATMULL POINTS
		glUseProgram(catmullShader->getProgramID());
		glBindVertexArray(VAO[1]);
		glPointSize(25);
		if (rightMouseButtonClicked)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, catPoints.size() * sizeof(float), catPoints.data(), GL_STATIC_DRAW);
			rightMouseButtonClicked = false;
		}
		glDrawArrays(GL_POINTS, 0, catPoints.size() / 3);	// 3 components per vertex

		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(2, VBO);

	glfwTerminate();
	return 0;
}

void ImageDisplayer::processInput(GLFWwindow *window)
{
	prevMouseX = mouseX;
	prevMouseY = mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//float scale = 1.0f;
	float deltaX = 0.0f;
	float deltaY = 0.0f;

	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	float openGlUnitsPerPixelX = 2.0f / windowWidth;
	float openGlUnitsPerPixelY = 2.0f / windowHeight;

	if (mouseInWindow && leftMouseButtonHeld)
		holdingImage = true;
	if(!leftMouseButtonHeld)
		holdingImage = false;
	if (holdingImage)
	{
		deltaX = (mouseX - prevMouseX) * openGlUnitsPerPixelX;
		deltaY = -(mouseY - prevMouseY) * openGlUnitsPerPixelY;	// img y axis is inverted compared to opengl
		//cout << "X: " << mouseX << " Y: " << mouseY << endl;
		//cout << "dX: " << deltaX << " dY: " << deltaY << endl;
	}

	if (rightMouseButtonClicked)
	{
		float catX = -1.0f + mouseX * openGlUnitsPerPixelX;
		float catY = 1.0f -  mouseY * openGlUnitsPerPixelY;
		cout << catX << "  " << catY << endl;
		catPoints.push_back(catX);
		catPoints.push_back(catY);
		catPoints.push_back(0);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, catPoints.size() * sizeof(float), catPoints.data());
		//glBindVertexArray(0);
		//rightMouseButtonClicked = false;

	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)	// zoom in
		scale *= 1.009f;	
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)	// zoom out
		scale *= 0.991f;	
	
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !c_KeyHeld)
	{
		c_KeyHeld = true;
		if (grayScale) 
			grayScale = false;
		else
		{
			grayScale = true;
			twoBitQuant = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		c_KeyHeld = false;

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !v_KeyHeld)
	{
		v_KeyHeld = true;
		if (twoBitQuant) 
			twoBitQuant = false;
		else
		{
			twoBitQuant = true;
			grayScale = false;			
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
		v_KeyHeld = false;	

	modelView = glm::translate(modelView, glm::vec3(deltaX, deltaY, 0));	
	//modelView = glm::scale(modelView, glm::vec3(scale, scale, 0));
	
}

void ImageDisplayer::adjustAspectRatio()
{
	glfwSetWindowAspectRatio(window, texture->getWidth(), texture->getHeight());
}

void ImageDisplayer::onCursorEnterCallback(GLFWwindow *window, int entered)
{
	if (entered) mouseInWindow = true;
	else mouseInWindow = false;
}