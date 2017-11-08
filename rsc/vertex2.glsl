#version 410 core
layout (location = 0) in vec3 aPos;

//out vec3 fragColor;

void main()
{
    //glPointSize(50);
    gl_Position = vec4(aPos, 1.0f);

}