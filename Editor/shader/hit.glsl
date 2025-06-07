HitPayload Miss() {
    HitPayload payload;
    payload.t = -1.0;
    return payload;
}

HitPayload ClosestHit(Ray ray, float t, int objectIndex) {
    HitPayload payload;
    payload.t = t;
    payload.objectIndex = objectIndex;
    Sphere s = spheres[objectIndex];

    vec3 hitPoint = ray.origin + t * ray.direction;
    vec3 normal = normalize(hitPoint - s.position);
    payload.worldPos = hitPoint;
    payload.worldNormal = normal;

    // Spherical UV mapping
    vec3 p = normalize(hitPoint - s.position);
    float u = 0.5 + atan(p.z, p.x) / (2.0 * PI);
    float v = 0.5 - asin(p.y) / PI;
    payload.uv = vec2(u, v);
    return payload;
}

HitPayload TraceRay(Ray ray) {
    float closestT = 1e20;
    int hitIndex = -1;
    for (int i = 0; i < sphereCount; ++i) {
        Sphere s = spheres[i];
        vec3 oc = ray.origin - s.position;
        float a = dot(ray.direction, ray.direction);
        float b = 2.0 * dot(oc, ray.direction);
        float c = dot(oc, oc) - s.radius * s.radius;
        float discriminant = b * b - 4.0 * a * c;
        if (discriminant > 0.0) {
            float t = (-b - sqrt(discriminant)) / (2.0 * a);
            if (t > 0.001 && t < closestT) {
                closestT = t;
                hitIndex = i;
            }
        }
    }

    if (hitIndex >= 0)
        return ClosestHit(ray, closestT, hitIndex);

    return Miss();
}
