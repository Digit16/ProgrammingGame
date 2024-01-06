#version 420

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 dim;
layout (location = 2) in int tex;
layout (location = 3) in int inv; // treated as bool

uniform vec2 window;
uniform float layer;

out vec2 g_dim;
out int g_tex;
out int g_inv; // treated as bool

vec2 map(vec2 value, vec2 min1, vec2 max1, vec2 min2, vec2 max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec2 map_from_pixel_to_screen(vec2 value) {

    ////////////////////////////////////////
    // (0, 0)               (window.x, 0) //
    //                                    //
    //            Pixel Space             //
    //                                    //
    // (0, window.y) (window.x, window.y) //
    ////////////////////////////////////////

    ////////////////////////////////////////
    // (-1, 1)                     (1, 1) //
    //                                    //
    //            Screen Space            //
    //                                    //
    // (-1, -1)                   (1, -1) //
    ////////////////////////////////////////

    return map(value, vec2(0, 0), window, vec2(-1, 1), vec2(1, -1));
}

void main(void) {

    vec2 screenPos = map_from_pixel_to_screen(pos);

    gl_Position = vec4(screenPos, layer, 1);
    
    g_dim = dim / window * 2;
    g_tex = tex;
    g_inv = inv;
}
