#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexture;

uniform mat4 modelView;
uniform float scale = 1.0;

out vec2 texCoord;

void main()
{
	//vec4 trans = modelView * vec4(aPos, 1.0f);
	//gl_Position = vec4(trans.xyz, 1/scale);
	gl_Position = modelView * vec4(aPos, 1.0);
	texCoord = aTexture;
}
