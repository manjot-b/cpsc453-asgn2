#ifndef IMAGEDISPLAYER_H
#define IMAGESDISPLAYER_H

#include <GLFW/glfw3.h>
#include "Texture.h"
#include "Shader.h"
#include <vector>

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
        
        void adjustAspectRatio();
};

#endif