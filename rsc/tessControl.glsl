#version 410 core

in vec3 tcPosition[];
out vec3 tePosition[];

layout (vertices = 2) out;

#define ID gl_InvocationID

void main()
{
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = 10;

    tePosition[ID] = tcPosition[ID]; 
}
