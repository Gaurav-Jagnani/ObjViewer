#version 330 core

layout(location=0) in vec3 vPos;

uniform mat4 P;
uniform mat4 MV;


void main()
{
	gl_Position = P * MV * vec4(vPos, 1);
}