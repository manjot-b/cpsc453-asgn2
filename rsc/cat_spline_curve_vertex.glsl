#version 410 core
layout (location = 0) in vec3 vPosition;

out vec3 tcPosition;
uniform float scale = 1.0;
uniform mat4 modelView;

void main()
{
    //glPointSize(50);
    //gl_Position = vec4(vPosition, 1.0f);
    vec4 trans = modelView * vec4(vPosition, 1.0);
    tcPosition = trans.xyz;
}