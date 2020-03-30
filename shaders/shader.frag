#version 330 core

smooth in vec2 uv;
uniform sampler2D textureMap;
uniform float useDefaultColor;

out vec4 fragColor;


void main()
{
	fragColor = mix(
		texture(textureMap, uv),
		vec4(1, 1, 1, 1),
		useDefaultColor
	);
}