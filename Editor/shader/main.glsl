#version 450 core
#include "common.glsl"

in vec2 vUV;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vUV, 0.0, 1.0); // placeholder: show UVs
}
