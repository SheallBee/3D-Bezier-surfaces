#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 inColor;

out vec4 outColor;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
	outColor = inColor;
};

#shader fragment
#version 330 core

in vec4 outColor;

layout(location = 0) out vec4 color;

void main()
{
	color = outColor;
};