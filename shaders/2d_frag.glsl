#version 420

#define TEX_NUM 32 // this is the max number of guaranteed textures

uniform sampler2D textures[TEX_NUM];
uniform ivec2 tilemapSize[TEX_NUM];

in vec2 f_texPos;
flat in int f_tex;
flat in int f_inv; // treated as bool

out vec4 pixelColor;


void main(void) {

    vec3 color;

    // g_tex contains index of texture and index of tile within the texture
    // TODO: change to two separate input arguments
    int textureIndex = f_tex % TEX_NUM;
    int tileIndex = f_tex / TEX_NUM;

    // get (x, y) tile position in tilemap 
    vec2 tilePos = vec2(tileIndex % tilemapSize[textureIndex].x, tileIndex / tilemapSize[textureIndex].x);

    // get (x, y) position in tilemap
    vec2 texPos = (f_texPos + tilePos) / tilemapSize[textureIndex];

    color = texture(textures[textureIndex], texPos).xyz;

    // invert color
    if (f_inv > 0) {
        color = 1 - color;
    }

    // change colors
    // TODO: make customizable
    if (color.r == 0) {
        color = vec3(0.13f);
    } else {
        color = vec3(0.67f);
    }

    pixelColor = vec4(color, 1.0);
}
