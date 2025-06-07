#version 460

#include "common.glsl"
#include "ray.glsl"
#include "scene.glsl"
#include "hit.glsl"

layout(location = 0) out vec4 FragColor;
in vec2 vUV;

uniform int frameIndex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 projectionInverse;
uniform mat4 viewInverse;

vec3 rayDirection(vec2 uv) {
    vec2 coord = uv * 2.0 - 1.0;
    vec4 target = projectionInverse * vec4(coord, -1.0, 1.0);
    vec3 rayDirection = vec3(viewInverse * vec4(normalize(vec3(target) / target.w), 0));
    return rayDirection;
}

void main() {
    FragColor = vec4(rayDirection(vUV), 1.0);
}
