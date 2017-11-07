#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexture;
uniform mat4 modelView;

out vec2 texCoord;

void main()
{
	gl_Position = modelView * vec4(aPos, 1.0);
	texCoord = aTexture;
}
