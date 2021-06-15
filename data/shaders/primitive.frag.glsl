#version 330 core
// Fragment shader for primitive
// Link with primitive vertex shader

// Input from vertex shader
in vec4 vary_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vary_color;
}
