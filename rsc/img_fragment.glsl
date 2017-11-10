#version 410 core
out vec4 fragColor;
in vec4 vertexColor;	// used for input from vertex shader
uniform vec4 uColor;			// used for input from program (CPU)

in vec2 texCoord;
uniform sampler2D texture1;
uniform bool grayScale = false;
uniform bool twoBitQuant = false;

float convertTo2Bit(float value)
{
	if (value >= 0 && value < 0.25)
		return 0;
	else if (value >= 0.25 && value < 0.5)
		return 0.33;
	else if (value >= 0.5 && value < 0.75)
		return 0.66;
	else
		return 1;
}

void main()
{
	//fragColor = vec4(0.75, 0.75, 0.75, 1.0);
	fragColor = texture(texture1, texCoord);

	if (grayScale)
	{
		//float grayValue = (fragColor.r + fragColor.g + fragColor.b) / 3;
		float grayValue = (fragColor.r + 2 * fragColor.g) * 0.33;
		fragColor = vec4(grayValue, grayValue, grayValue, fragColor.a);
	}
	else if (twoBitQuant)
	{
		float r = convertTo2Bit(fragColor.r);
		float g = convertTo2Bit(fragColor.g);
		float b = convertTo2Bit(fragColor.b);
		fragColor = vec4(r, g, b, fragColor.a); 
	}
		
}

