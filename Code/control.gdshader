shader_type canvas_item;

uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_linear;

vec2 warp(vec2 uv) {
    vec2 delta = uv - 0.5;
    float l = length(delta);
    return uv + delta * l * l / 2.0;
}

vec3 border(vec2 uv, vec3 col) {
    uv -= 0.5;
    uv *= 2.0;
    
    vec2 uv2 = uv*uv;
    vec2 uv4 = uv2*uv2;
    vec2 uv16 = uv4*uv4;
    
	float border = 1.0 - length(uv16);
	
	vec3 edge_color = vec3(0.164, 0.152, 0.19);
	float blur_size = 0.20;
	float edge_size = 0.10;
	float edge_blur_width = 0.04;
	
	float edge_center = blur_size - edge_size / 2.0;
	
	if (border < blur_size) {
		if (border > blur_size - edge_blur_width) {
			float change = smoothstep(blur_size - edge_blur_width, blur_size, border);
			return (1.0 - change) * edge_color;
		} else {
			float change = smoothstep(blur_size - 5.0, blur_size - edge_size + edge_blur_width, border);
			return change * edge_color;
		}
	}
	
    return col * smoothstep(0.0, 0.64, border - blur_size);
}

void fragment()
{
	vec2 uv = warp(SCREEN_UV); // change to SCREEN_UV if required
	vec2 text_uv = uv;
	

	vec4 text;
	text.rgb = texture(SCREEN_TEXTURE, text_uv).rgb;
	text.a = 1.0;

    float apply = abs(sin(uv.y * 400.0)*0.5*0.25);
    vec3 col = vec3(mix(texture(SCREEN_TEXTURE,uv).rgb,vec3(0.0),apply));
	text.rgb = col;

	uv = warp(UV);

	text.rgb = border(uv, text.rgb);

	COLOR = text;
}