#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
uniform mat4 projection;
uniform mat4 modelview;

out float sampleExtraOutput;
out vec2 UV;
void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
    sampleExtraOutput = 1.0f;
	UV = vertexUV;
}