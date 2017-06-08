#version 330 core

in float sampleExtraOutput;
in vec2 UV;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	vec3 texcolor = texture(ourTexture, UV).rgb;
    color = vec4(texcolor.x * sampleExtraOutput, texcolor.y * sampleExtraOutput, texcolor.z * sampleExtraOutput, 1.0);
}