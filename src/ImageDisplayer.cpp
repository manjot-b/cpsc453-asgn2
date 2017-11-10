#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ImageDisplayer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
using namespace std;

ImageDisplayer::ImageDisplayer(const char* _fileName) : 
	modelView(1.0f), scale(1.0f), prevMouseX(0), prevMouseY(0), mouseX(0), mouseY(0),
	mouseInWindow(false), leftMouseButtonHeld(false), rightMouseButtonClicked(false), 
	holdingImage(false), grayScale(false), twoBitQuant(false), c_KeyHeld(false), v_KeyHeld(false)
{
    squareVertices = {
                //Triangle coords   	// texture coords
                -1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
                1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
                -1.0f, 1.0f, 0.0f,		0.0f, 1.0f		
				};
	
	squareIndices = {
					0, 1, 2,
					0, 2, 3
					};
	catVertices = {};
	catIndices = {};
	initWindow();
	texture = new Texture(_fileName);
	adjustAspectRatio(); 
	imgShader = new Shader("rsc/img_vertex.glsl", "rsc/img_fragment.glsl");
	imgShader->link();
	catmullShader = new Shader("rsc/cat_spline_curve_vertex.glsl", "rsc/cat_spline_fragment.glsl");
	catmullShader->addShader("rsc/tess_control.glsl", GL_TESS_CONTROL_SHADER);
	catmullShader->addShader("rsc/tess_evaluation.glsl", GL_TESS_EVALUATION_SHADER);
	catmullShader->link();
	catmullPointShader = new Shader("rsc/cat_spline_point_vertex.glsl", "rsc/cat_spline_fragment.glsl");
	catmullPointShader->link();
}

ImageDisplayer::~ImageDisplayer()
{
	delete texture;
	delete imgShader;
	delete catmullShader;
	delete catmullPointShader;
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
	
	glGenBuffers(2, VBO); // gen 2 buffer and store id in VBO
	glGenBuffers(2, EBO);
	glGenVertexArrays(2, VAO);	// one is for texture, the other is for catmull rom points
	glBindVertexArray(VAO[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, squareVertices.size() * sizeof(float), squareVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, squareIndices.size() * sizeof(float), squareIndices.data(), GL_STATIC_DRAW);

	int TEXTURE_COMPONENTS = 2;
	int VERTEX_COMPONENTS = 3;
	int COMPONENTS_PER_VERTEX = TEXTURE_COMPONENTS + VERTEX_COMPONENTS;
	int STRIDE = COMPONENTS_PER_VERTEX * sizeof(float);
	//position attributes
	glVertexAttribPointer(0, VERTEX_COMPONENTS, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
	glEnableVertexAttribArray(0);
	//texture attributes
	glVertexAttribPointer(1, TEXTURE_COMPONENTS, GL_FLOAT, GL_FALSE, STRIDE, (void*)(VERTEX_COMPONENTS * sizeof(float)));
	glEnableVertexAttribArray(1);

	// CATMULL ROM POINTS
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, catVertices.size() * sizeof(float), catVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, catIndices.size() * sizeof(float), catIndices.data(), GL_STATIC_DRAW);

	COMPONENTS_PER_VERTEX = VERTEX_COMPONENTS;
	STRIDE = COMPONENTS_PER_VERTEX * sizeof(float);
	// points attribs
	glVertexAttribPointer(0, VERTEX_COMPONENTS, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
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

		// DRAW IMAGE AS A TEXTURE
		glUseProgram(imgShader->getProgramID());
		imgShader->setUniformMatrix4fv("modelView", modelView);
		imgShader->setUniform1i("grayScale", grayScale);
		imgShader->setUniform1i("twoBitQuant", twoBitQuant);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getID());

		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, squareIndices.size(), GL_UNSIGNED_INT, 0);
		
		// DRAW CATMULL POINTS AND CURVE
		glBindVertexArray(VAO[1]);
		if (catVertices.size() > 0)
		{	
			if (rightMouseButtonClicked)
			{
				glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
				glBufferData(GL_ARRAY_BUFFER, catVertices.size() * sizeof(float), catVertices.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, catIndices.size() * sizeof(float), catIndices.data(), GL_STATIC_DRAW);
				rightMouseButtonClicked = false;
			}
			
			glUseProgram(catmullPointShader->getProgramID());
			glPointSize(5*scale);
			glDrawArrays(GL_POINTS, 0, catVertices.size() / 3);
			catmullPointShader->setUniformMatrix4fv("modelView", modelView);

			if (catIndices.size() >= 4)	// need at least 4 points to draw catmull spline
			{
				glUseProgram(catmullShader->getProgramID());
				catmullShader->setUniformMatrix4fv("modelView", modelView);		
				glPatchParameteri(GL_PATCH_VERTICES, 4);
				glDrawElements(GL_PATCHES, catIndices.size(), GL_UNSIGNED_INT, 0);
			}
		}
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, EBO);
	glDeleteBuffers(2, VBO);

	glfwTerminate();
	return 0;
}

void ImageDisplayer::processInput(GLFWwindow *window)
{
	prevMouseX = mouseX;
	prevMouseY = mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float scale = 1.0f;
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
	}

	if (rightMouseButtonClicked)
	{
		glm::mat4 inv = glm::inverse(modelView);
		glm::mat4 test(1.0f);
		glm::vec4 points = inv * glm::vec4(-1.0f + mouseX * openGlUnitsPerPixelX,
											1.0f -  mouseY * openGlUnitsPerPixelY,
											0, 1);
		float catX = points.x;
		float catY = points.y;
		catVertices.push_back(catX);
		catVertices.push_back(catY);
		catVertices.push_back(0);		// z coord
		if (catIndices.size() < 4)		// need 4 points for the catmull spline
			catIndices.push_back(catIndices.size());
		else							// the new splines takes last 3 points and 1 new one
		{
			unsigned int size = catIndices.size() - 1;
			for (unsigned int i = 0; i < 4; i++)
			{
				catIndices.push_back(catIndices[size-3] + (i + 1));
			}
		}
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)	// zoom in
		scale *= 1.009f;	
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)	// zoom out
		scale *= 0.991f;	
	
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !c_KeyHeld)	// toggle grayscale
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

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !v_KeyHeld)	// toggle 2-bit quantization
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
	modelView = glm::scale(modelView, glm::vec3(scale, scale, scale));
								
}

void ImageDisplayer::adjustAspectRatio()
{
	glfwSetWindowAspectRatio(window, texture->getWidth(), texture->getHeight());
}
