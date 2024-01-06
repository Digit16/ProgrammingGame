#version 330 core

out vec2 f_texPos;

vec2 trianglePoints[6] = vec2[6](
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),

    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0)
);

void main() {
    vec2 vertexPos = trianglePoints[gl_VertexID];

    gl_Position = vec4(vertexPos, 0.0, 1.0);
    f_texPos = 0.5 * (vertexPos + vec2(1.0, 1.0));
}