#define PI 3.14159265358979323846

uint PCG_Hash(uint x) {
    uint state = x * 747769405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float RandomFloat(inout uint seed) {
    seed = PCG_Hash(seed);
    return float(seed) / float(0xffffffffu);
}

vec3 RandomInUnitSphere(inout uint seed) {
    return normalize(vec3(
        RandomFloat(seed) * 2.0 - 1.0,
        RandomFloat(seed) * 2.0 - 1.0,
        RandomFloat(seed) * 2.0 - 1.0
    ));
}

vec3 ToSkyColor(vec3 dir) {
    float t = (dir.y + 1.0) * 0.5;
    vec3 topColor = vec3(0.2, 0.4, 0.9);
    vec3 horizonColor = vec3(0.8, 0.8, 1.0);
    vec3 bottomColor = vec3(0.4, 0.3, 0.2);
    if (t > 0.5) {
        float nt = (t - 0.5) * 2.0;
        return mix(horizonColor, topColor, nt);
    } else {
        float nt = t * 2.0;
        return mix(bottomColor, horizonColor, nt);
    }
}
