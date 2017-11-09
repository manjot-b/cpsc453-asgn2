#version 410 core
layout (location = 0) in vec3 vPosition;

out vec3 tcPosition;

void main()
{
    //glPointSize(50);
    gl_Position = vec4(vPosition, 1.0f);
}