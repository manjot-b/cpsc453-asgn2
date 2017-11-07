#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexture;
out vec4 vertexColor;
uniform mat4 transform;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	texCoord = aTexture;
}
