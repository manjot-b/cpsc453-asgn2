/*
*   Displays images and draws catmull points
*
*/

#ifndef IMAGEDISPLAYER_H
#define IMAGESDISPLAYER_H

#include <GLFW/glfw3.h>
#include "Texture.h"
#include "Shader.h"
#include <vector>
#include <glm/glm.hpp>

class ImageDisplayer
{
    public: 
        ImageDisplayer(const char *_fileName);
        ~ImageDisplayer();
        
        /*
        *   run the program and exits once
        *   the windows is closed
        */
        int run();          
    
    private:
        /*
        *   Initializes the glfw window
        */
        int initWindow();
        GLFWwindow *window;
        GLuint VBO[2], VAO[2], EBO[2];
        char *fileName;                                 // the image file name
        Texture *texture;
        Shader *imgShader;                              // shaded for the image
        Shader *catmullShader;                          // shader the catmull curve
        Shader *catmullPointShader;                     // shader for the catmull points
        std::vector<float> squareVertices;              // verticies of the square the texture is drawn on
        std::vector<unsigned int> squareIndices;        
        glm::mat4 modelView;                            // sent to the shaders to transform
        std::vector<float> catVertices;                 //  vertices of the catmull points
        std::vector<unsigned int> catIndices;
        float scale;
        double prevMouseX, prevMouseY, mouseX, mouseY;  
        bool mouseInWindow;
        bool leftMouseButtonHeld;
        bool rightMouseButtonClicked;
        bool holdingImage;
        bool grayScale;
        bool twoBitQuant;
        bool c_KeyHeld;
        bool v_KeyHeld;

        /*
        *   Process the input made by the user
        */
        void processInput(GLFWwindow *window);
        void adjustAspectRatio();
};

#endif