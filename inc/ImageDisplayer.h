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
        int initWindow();        
        int run();
    
    private:
        GLFWwindow *window;
        GLuint VBO, VAO, EBO;
        char *fileName;
        Texture *texture;
        Shader *shader;
        std::vector<float> verticies;
        std::vector<unsigned int> indicies;
        glm::mat4 modelView;
        double prevMouseX, prevMouseY, mouseX, mouseY;
        bool mouseInWindow;
        bool leftMouseButtonHeld;
        bool holdingImage;

        void processInput(GLFWwindow *window);
        void adjustAspectRatio();
        void onCursorEnterCallback(GLFWwindow *window, int entered);
};

#endif