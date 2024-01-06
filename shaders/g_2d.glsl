#version 420

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2 g_dim[];
in int g_tex[];
in int g_inv[];

out vec2 f_texPos;
flat out int f_tex;
flat out int f_inv; // treated as bool


void main(void) {

	vec4 pos = gl_in[0].gl_Position;
	vec2 dim = g_dim[0];

	f_tex = g_tex[0];
	f_inv = g_inv[0];

	// draw 4 corners of 2d tile

	gl_Position = pos + vec4(0, 0, 0, 0);
	f_texPos = vec2(0, 0);
	EmitVertex();

	gl_Position = pos + vec4(dim.x, 0, 0, 0);
	f_texPos = vec2(1, 0);
	EmitVertex();

	gl_Position = pos + vec4(0, -dim.y, 0, 0);
	f_texPos = vec2(0, 1);
	EmitVertex();

	gl_Position = pos + vec4(dim.x, -dim.y, 0, 0);
	f_texPos = vec2(1, 1);
	EmitVertex();

	EndPrimitive();

}