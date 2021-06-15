#version 330 core
// Fragment shader for texture2D
// Link with texture2D vertex shader

// Input from vertex shader
in vec4 vary_color;
in vec2 vary_texCoord;

// Input through uniforms
uniform sampler2D u_texture2D;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vary_color * texture(u_texture2D, vary_texCoord);
}
