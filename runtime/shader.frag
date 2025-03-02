#version 400 core
out vec4 FragColor;
in vec2 UV;

uniform float u_time;
uniform int u_frame;
uniform vec2 u_resolution;
uniform float u_aspectRatio;

void main()
{
    vec2 aspectUV = vec2(UV.x, UV.y);
    vec3 rayOrigin = vec3(0.0f, 0.0f, 1.0f);
    vec3 rayDirection = vec3(aspectUV.x, aspectUV.y, -1.0f);
    float radius = 0.5f;

    float a = dot(rayDirection, rayDirection);
    float b = 2.0f * dot(rayOrigin, rayDirection);
    float c = dot(rayOrigin, rayOrigin) - radius * radius;
    float discriminent = b * b - 4.0f * a * c;

    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if(discriminent < 0.0f){
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    float t0 = (-b + sqrt(discriminent)) / (2.0f * a);
    float closestT = (-b - sqrt(discriminent)) / (2.0f * a);

    vec3 hitPoint = rayOrigin + rayDirection * closestT;
    vec3 normal = normalize(hitPoint);

    vec3 lightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));
    float d = max(dot(normal, -lightDir), 0.0f); // cos(angle)

    vec4 sphereColor = vec4(1.0, 0.0, 1.0f, 1.0f);
    FragColor = sphereColor * d;
    //FragColor = vec4(normal * 0.5f + 0.5f, 1.0f);
}
