#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoord;

out vec3 outColour;
out vec2 outTexCoord;

uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 projMtx;

void main()
{
	gl_Position = projMtx * viewMtx * modelMtx * vec4(aPos, 1.0);
	outColour = aColour;
	outTexCoord = aTexCoord;
}