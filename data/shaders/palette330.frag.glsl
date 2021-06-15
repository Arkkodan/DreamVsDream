#version 330 core
// Fragment shader for fighters
// Link with texture2D vertex shader

// Input from vertex shader
in vec4 vary_color; // Ignore alpha in this fragment shader
in vec2 vary_texCoord;

// Input through uniforms
uniform sampler2D u_texture2D;
uniform sampler2D u_palette;

// Resultant alpha
uniform float u_alpha;

// Linearly mix flat color and texture
// uniform vec3 color;
uniform float u_pct; // Percent of flat color

// Shift down the color palette texture (256x1)
uniform float u_shift;

// How many times larger a pixel is
uniform int u_pixel;

// varying vec2 coord;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
	// Pixel size in float format
	float pixelWidth = float(u_pixel) / 2048.0;

	// Pixelate x and y
	float x = floor(vary_texCoord.s / pixelWidth) * pixelWidth + pixelWidth / 2.0;
	float y = floor(vary_texCoord.t / pixelWidth) * pixelWidth + pixelWidth / 2.0;

	// Obtain palette index
	float palcolor = texture2D(u_texture2D, vec2(x, y)).r;

	if(palcolor == 0.0)
		// If palette index is 0, draw transparent
		out_color = vec4(0.0, 0.0, 0.0, 0.0);
	else
	{
		// Draw the calculated color for the fragment
		out_color = vec4(vary_color.rgb, 1.0) * u_pct + texture2D(u_palette, vec2(palcolor + u_shift, 0)) * (1.0 - u_pct);
		out_color.a = u_alpha;
	}
}
