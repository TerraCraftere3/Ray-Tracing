struct Ray {
    vec3 origin;
    vec3 direction;
};

Ray CreateRay(vec3 origin, vec3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = normalize(direction);
    return ray;
}
