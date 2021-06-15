#version 330 core
// Vertex shader for primitive
// Link with primitive fragment shader

// Input through bound data
layout(location = 0) in vec2 in_position;

// Input through uniforms
uniform mat4 u_mvp;
uniform vec4 u_color;

// Output to fragment shader
out vec4 vary_color;

void main()
{
	gl_Position = u_mvp * vec4(in_position, 0.0, 1.0);
	vary_color = u_color;
}
