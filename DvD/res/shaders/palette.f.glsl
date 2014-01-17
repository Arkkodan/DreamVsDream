uniform sampler2D texture;
uniform sampler2D palette;

uniform float alpha;

uniform vec3 color;
uniform float pct;

uniform float shift;

uniform int pixel;

varying vec2 coord;

void main()
{
	float pixelWidth = float(pixel)/2048.0;
	float x = floor(coord.x/pixelWidth)*pixelWidth + pixelWidth/2.0;
	float y = floor(coord.y/pixelWidth)*pixelWidth + pixelWidth/2.0;
	float palcolor = texture2D(texture, vec2(x, y)).r;

	if(palcolor == 0.0)
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
	{
		gl_FragColor = vec4(color.r, color.g, color.b, 1.0) * pct + texture2D(palette, vec2(palcolor+shift, 0)) * (1.0-pct);
		gl_FragColor.a = alpha;
	}
}