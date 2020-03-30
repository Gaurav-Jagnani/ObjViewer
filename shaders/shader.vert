#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec2 vUV;

uniform mat4 P;
uniform mat4 MV;

smooth out vec2 uv;


void main()
{
	gl_Position = P * MV * vec4(vPos, 1);
	uv = vUV;
}