#version 330

layout(location = 0) in vec3 aPosition;

uniform mat4 uMVP;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;

out vec3 vPos;

void main()
{
	gl_Position = uMVP * vec4(aPosition, 1.0);
	vPos = (uModelMatrix * vec4(aPosition, 1.0)).xyz;
}