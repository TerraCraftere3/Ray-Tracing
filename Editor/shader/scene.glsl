struct Material {
    vec3 albedo;
    vec3 emission;
    float roughness;
};

struct Sphere {
    vec3 position;
    float radius;
    int materialIndex;
};

struct HitPayload {
    float t;
    int objectIndex;
    vec3 worldNormal;
    vec3 worldPos;
    vec2 uv;
};

// Scene data to be filled by host via uniforms or SSBOs
layout(std140, binding = 0) buffer Spheres {
    Sphere spheres[];
};

layout(std140, binding = 1) buffer Materials {
    Material materials[];
};

uniform int sphereCount;
uniform int materialCount;
uniform vec2 resolution;
