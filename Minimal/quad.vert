#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 t;
uniform vec3 eyepos;
uniform vec3 quadcenter;
uniform vec3 quadnormal;
out float sampleExtraOutput;
out vec2 UV;
void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	vec4 temp = t * vec4(position.x, position.y, position.z, 1.0);
	UV = vertexUV;
	vec3 dist = eyepos - vec3(temp.xyz);
	dist = normalize(dist);
	sampleExtraOutput = abs(dot(dist, quadnormal));
	sampleExtraOutput = sampleExtraOutput * sampleExtraOutput * sampleExtraOutput;
}