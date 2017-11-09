#version 410 core

in vec3 tePosition[];

layout (isolines, equal_spacing) in;

void main()
{
    vec3 p0 = tePosition[0];
    vec3 p1 = tePosition[1];

    float u = gl_TessCoord.x;

    gl_Position = vec4((1.0-u)*p0 + u*p1, 1.0);
}