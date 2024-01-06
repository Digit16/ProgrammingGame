#version 330 core

in vec2 f_texPos;

uniform sampler2D colorBuffer;

out vec4 pixelColor;

// TODO: rewrite/change
vec2 curveRemapUV(vec2 uv) {

    vec2 curvature = vec2(4.0);

    uv = uv * 2.0-1.0;
    vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

// TODO: rewrite/change
float vignetteMod(inout vec3 color, vec2 uv) {

    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(32.0 * vignette, 0.3), 0.0, 1.0);
    return vignette;
}

// TODO: rewrite/change
float scanlineMod(inout vec3 color, vec2 uv) {

    float scanline = clamp(0.95 + 0.05 * cos(3.14 * 240.0), 0.0, 1.0);
    float grille = 0.75 + 0.25 * clamp(1.5 * cos(3.14 * uv.x * 800 * 0.8), 0.0, 1.0);
    return scanline * grille * 1.5;
}

bool isTextureInBounds(vec2 pos) {
    return pos.x >= 0.0 && pos.x <= 1.0 && pos.y >= 0.0 && pos.y <= 1.0;
}

void main() {

    vec2 remappedUV = curveRemapUV(f_texPos);
    vec4 pixel = texture(colorBuffer, remappedUV);

    pixel.xyz *= vignetteMod(pixel.xyz, remappedUV);
    pixel.xyz *= scanlineMod(pixel.xyz, remappedUV);

    if (!isTextureInBounds(remappedUV)) {
        pixel = vec4(0.0);
    } 

    if (pixel.a == 0) {
        discard;
    }

    pixelColor = vec4(pixel.xyz, 1.0);
}