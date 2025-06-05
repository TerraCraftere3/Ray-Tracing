#include "Renderer.h"
#include <execution>
#include <glm/ext/scalar_constants.hpp>

using namespace glm;

#define HitPayload Renderer::HitPayload

namespace Utils {
	static uint32_t ConvertToRGBA(const vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static uint32_t PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747769405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed) {
		seed = PCG_Hash(seed);
		return (float)seed / (float)UINT32_MAX;
	}

	static vec3 RandomInUnitSphere(uint32_t& seed)
	{
		return normalize(vec3(
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f
		));
	}
}

#define MT

void Renderer::Render(const Camera& camera, const Scene& scene)
{
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, sizeof(vec4) * m_FinalImage->GetWidth() * m_FinalImage->GetHeight());

	auto ProcessPixel = [this](uint32_t x, uint32_t y) {
		int index = y * m_FinalImage->GetWidth() + x;
		vec4 color = PerPixel(x, y);
		m_AccumulationData[index] += color;

		vec4 accumulatedColor = m_AccumulationData[index];
		accumulatedColor /= (float)m_FrameIndex; // Average

		accumulatedColor = clamp(accumulatedColor, vec4(0.0f), vec4(1.0f));
		m_ImageData[index] = Utils::ConvertToRGBA(accumulatedColor);
		};

	if (m_Settings.MultiThreaded) {
		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[this, &ProcessPixel](uint32_t y) {
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
					[this, &ProcessPixel, y](uint32_t x) {
						ProcessPixel(x, y);
					});
			});
	}
	else {
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
				ProcessPixel(x, y);
			}
		}
	}


	m_FinalImage->SetData(m_ImageData);
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Create(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Image>(width, height);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	// rayOrigin = origin of the ray
	// rayDirection = direction of the ray
	// r = radius
	// t = hit distance

	int closestSphereIndex = -1;
	float hitDistance = FLT_MAX;
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float radius = sphere.Radius;
		vec3 origin = ray.Origin - sphere.Position;

		float a = dot(ray.Direction, ray.Direction);
		float b = 2.0f * dot(origin, ray.Direction);
		float c = dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0)
			continue;

		float cloestT = (-b - sqrt(discriminant)) / (2.0f * a);
		if (cloestT > 0.0f && cloestT < hitDistance) {
			closestSphereIndex = (int)i;
			hitDistance = cloestT;
		}
	}

	if (closestSphereIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphereIndex);


}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	vec3 light(0);
	vec3 contribution(1.0f);

	uint32_t seed = x + y * m_FinalImage->GetWidth();
	seed *= m_FrameIndex;

	for (int i = 0; i < 5; i++) {
		seed += i;

		HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f) {
			vec3 topColor(0.2, 0.4, 0.9);
			vec3 horizonColor(0.8, 0.8, 1.0);
			vec3 bottomColor(0.4, 0.3, 0.2);
			vec3 skycolor;
			float t = (ray.Direction.y + 1.0f) * 0.5f; // Converting ray.Direction.y to [0, 1]
			if (t > 0.5) {
				float nt = (t - 0.5f) * 2.0f; // Normalize t to [0, 1]
				skycolor = mix(horizonColor, topColor, nt);
			}
			else {
				float nt = t * 2.0f; // Normalize t to [0, 1]
				skycolor = mix(bottomColor, horizonColor, nt);
			}
			light += skycolor * contribution;
			break;
		}

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& mat = m_ActiveScene->Materials[sphere.MaterialIndex];

		light += mat.GetEmission();
		if (length(mat.GetEmission()) > 0.0f)
			break;

		contribution *= vec3(mat.Albedo.Sample(payload.UV));

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = reflect(ray.Direction, payload.WorldNormal + mat.Roughness * Utils::RandomInUnitSphere(seed));
	}

	return vec4(light, 1);
}


HitPayload Renderer::ClosestHit(const Ray& ray, float HitDistance, int ObjectIndex)
{
	HitPayload payload;
	payload.HitDistance = HitDistance;
	payload.ObjectIndex = ObjectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[ObjectIndex];

	vec3 origin = ray.Origin - closestSphere.Position;
	vec3 localPosition = origin + HitDistance * ray.Direction;
	payload.WorldPosition = origin + HitDistance * ray.Direction;
	payload.WorldNormal = normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position; // Convert to world space

	vec3 p = normalize(localPosition);
	float u = 0.5f + atan2(p.z, p.x) / (2.0f * glm::pi<float>());
	float v = 0.5f - asin(p.y) / glm::pi<float>();
	payload.UV = vec2(u, v);

	return payload;
}

HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
	payload.HitDistance = -1.0f; // Indicating no hit
	return payload;
}
