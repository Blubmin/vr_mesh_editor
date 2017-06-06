#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in int aSelected;

uniform mat4 uMVP;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uDiffuseColor;

out vec3 vPos;
out vec3 vDiff;

void main()
{
	gl_Position = uMVP * vec4(aPosition, 1.0);
	vPos = (uModelMatrix * vec4(aPosition, 1.0)).xyz;
	vDiff = aSelected == 1 ? vec3(1, .5, 0) : uDiffuseColor;
}