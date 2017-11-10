#version 410 core

in vec3 tePosition[];

layout (isolines, equal_spacing) in;
uniform float scale = 1.0;

void main()
{
    vec3 p0 = tePosition[0];
    vec3 p1 = tePosition[1];
    vec3 p2 = tePosition[2];
    vec3 p3 = tePosition[3];

    float u = gl_TessCoord.x;

    float b0 = (-u + 2.0*u*u - u*u*u) / 2.0;
    float b1 = (2.0  - 5.0*u*u + 3.0*u*u*u) / 2.0;
    float b2 = (u + 4.0*u*u - 3.0*u*u*u) / 2.0;
    float b3 = (-u*u + u*u*u) / 2.0;


    gl_Position = vec4(b0*p0 + b1*p1 + b2*p2 + b3*p3, 1 / scale);
}