#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vUV;

uniform mat4 P;
uniform mat4 MV;
uniform mat3 N;

smooth out vec3 viewSpacePos;
smooth out vec3 viewSpaceNormal;
smooth out vec2 uv;


void main()
{
	viewSpacePos = (MV * vec4(vPos, 1)).xyz;
	viewSpaceNormal = N * vNormal;
	uv = vUV;

	gl_Position = P * vec4(viewSpacePos, 1);
}