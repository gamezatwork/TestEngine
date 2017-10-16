#version 330 core
out vec4 FragColor;

in vec3 outColour;
in vec2 outTexCoord;

uniform sampler2D Texture;

void main()
{
	FragColor = texture(Texture,outTexCoord);
}