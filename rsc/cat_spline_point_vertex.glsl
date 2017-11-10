#version 410 core
layout (location = 0) in vec3 vPosition;

uniform float scale = 1.0;
uniform mat4 modelView;

void main()
{
    //vec4 trans = modelView * vec4(vPosition, 1.0f);
    //gl_Position = vec4(trans.xyz, 1 / scale);
    gl_Position = modelView * vec4(vPosition, 1.0);
}