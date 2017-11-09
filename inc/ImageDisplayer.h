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
        GLuint VBO[2], VAO[2];
        GLuint EBO;
        char *fileName;
        Texture *texture;
        Shader *imgShader;
        Shader *catmullShader;
        Shader *catmullPointShader;
        std::vector<float> verticies;
        std::vector<unsigned int> indicies;
        glm::mat4 modelView;
        std::vector<float> catPoints;
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

        void processInput(GLFWwindow *window);
        void adjustAspectRatio();
        void onCursorEnterCallback(GLFWwindow *window, int entered);
};

#endif