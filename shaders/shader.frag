#version 330 core

smooth in vec3 viewSpacePos;
smooth in vec3 viewSpaceNormal;
smooth in vec2 uv;
uniform mat4 MV;
uniform sampler2D textureMap;
uniform float useDefaultColor;
uniform vec3 lightPos;

out vec4 fragColor;


void main()
{
	vec4 viewSpaceLightPos = MV * vec4(lightPos, 1);
	vec3 lightVec = viewSpaceLightPos.xyz - viewSpacePos;
	lightVec = normalize(lightVec);

	float diffuse = max(dot(viewSpaceNormal, lightVec), 0);

	fragColor = mix(
		texture(textureMap, uv),
		vec4(1, 1, 1, 1),
		useDefaultColor
	);
	fragColor = diffuse * fragColor +
				vec4(0.4, 0.4, 0.4, 1) * fragColor;
}