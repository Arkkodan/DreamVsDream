#version 330 core
// Vertex shader for texture2D
// Link with either texture2D or palette fragment shaders

// Input through bound data
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texCoord;

// Input through uniforms
uniform mat4 u_mvp;
uniform vec4 u_color;

// Output to fragment shader
out vec4 vary_color;
out vec2 vary_texCoord;

void main(void)
{
	gl_Position = u_mvp * vec4(in_position, 0.0, 1.0);
	vary_color = u_color;
	vary_texCoord = in_texCoord;
}
