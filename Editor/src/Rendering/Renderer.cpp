#include "Renderer.h"
#include <execution>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/matrix_transform.hpp>

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

glm::mat4 GetTransform(const Mesh& mesh)
{
	glm::mat4 transform = glm::mat4(1.0f);

	// Apply scale, then rotation, then translation (TRS order)
	transform = glm::translate(transform, mesh.Position);

	transform = glm::rotate(transform, mesh.Rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, mesh.Rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, mesh.Rotation.z, glm::vec3(0, 0, 1));

	transform = glm::scale(transform, mesh.Scale);

	return transform;
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestSphereIndex = -1;
	int closestMeshIndex = -1;
	int closestTriangleIndex = -1;

	float closestT = FLT_MAX;
	float triU = 0.0f, triV = 0.0f;

	// --- Sphere intersection ---
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		vec3 origin = ray.Origin - sphere.Position;

		float a = dot(ray.Direction, ray.Direction);
		float b = 2.0f * dot(origin, ray.Direction);
		float c = dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			continue;

		float t = (-b - sqrt(discriminant)) / (2.0f * a);
		if (t > 0.0f && t < closestT) {
			closestT = t;
			closestSphereIndex = (int)i;
			closestMeshIndex = -1;
			closestTriangleIndex = -1;
		}
	}

	// --- Triangle mesh intersection ---
	for (size_t meshIdx = 0; meshIdx < m_ActiveScene->Meshes.size(); meshIdx++) {
		const Mesh& mesh = m_ActiveScene->Meshes[meshIdx];

		for (size_t i = 0; i < mesh.Indices.size(); i += 3) {
			uint32_t i0 = mesh.Indices[i];
			uint32_t i1 = mesh.Indices[i + 1];
			uint32_t i2 = mesh.Indices[i + 2];

			glm::mat4 transform = GetTransform(mesh);
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

			glm::vec3 v0 = glm::vec3(transform * glm::vec4(mesh.Vertices[i0], 1.0f));
			glm::vec3 v1 = glm::vec3(transform * glm::vec4(mesh.Vertices[i1], 1.0f));
			glm::vec3 v2 = glm::vec3(transform * glm::vec4(mesh.Vertices[i2], 1.0f));

			// Apply transform if needed here (Position, Rotation, Scale) — omitted for now

			const float EPSILON = 0.000001f;
			vec3 edge1 = v1 - v0;
			vec3 edge2 = v2 - v0;
			vec3 h = cross(ray.Direction, edge2);
			float a = dot(edge1, h);

			if (fabs(a) < EPSILON)
				continue;

			float f = 1.0f / a;
			vec3 s = ray.Origin - v0;
			float u = f * dot(s, h);
			if (u < 0.0f || u > 1.0f)
				continue;

			vec3 q = cross(s, edge1);
			float v = f * dot(ray.Direction, q);
			if (v < 0.0f || u + v > 1.0f)
				continue;

			float t = f * dot(edge2, q);
			if (t > EPSILON && t < closestT) {
				closestT = t;
				closestSphereIndex = -1;
				closestMeshIndex = (int)meshIdx;
				closestTriangleIndex = (int)i;
				triU = u;
				triV = v;
			}
		}
	}

	// Dispatch closest hit
	if (closestSphereIndex >= 0)
		return ClosestHit(ray, closestT, closestSphereIndex);

	if (closestMeshIndex >= 0)
		return ClosestHit(ray, closestT, closestMeshIndex, closestTriangleIndex, triU, triV);

	return Miss(ray);
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
			//light += contribution;
			break;
		}

		const Material& mat = (payload.MeshIndex >= 0)
			? m_ActiveScene->Materials[m_ActiveScene->Meshes[payload.MeshIndex].MaterialIndex]
			: m_ActiveScene->Materials[m_ActiveScene->Spheres[payload.ObjectIndex].MaterialIndex];

		light += mat.GetEmission();
		if (length(mat.GetEmission()) > 0.0f)
			break;

		contribution *= vec3(mat.Albedo.Sample(payload.UV));
		//contribution *= vec3(payload.MeshIndex >= 0);

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = reflect(ray.Direction, payload.WorldNormal + mat.Roughness.Sample(payload.UV).x * Utils::RandomInUnitSphere(seed));
	}

	return vec4(light, 1);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int meshIndex, int triangleIndex, float u, float v)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.MeshIndex = meshIndex;

	const Mesh& mesh = m_ActiveScene->Meshes[meshIndex];
	const Material& material = m_ActiveScene->Materials[mesh.MaterialIndex];

	// Triangle indices
	uint32_t i0 = mesh.Indices[triangleIndex];
	uint32_t i1 = mesh.Indices[triangleIndex + 1];
	uint32_t i2 = mesh.Indices[triangleIndex + 2];

	// Vertex data
	const glm::vec3& v0 = mesh.Vertices[i0];
	const glm::vec3& v1 = mesh.Vertices[i1];
	const glm::vec3& v2 = mesh.Vertices[i2];

	// World-space hit position
	glm::vec3 position = ray.Origin + hitDistance * ray.Direction;

	// Compute barycentric coordinate for w
	float w = 1.0f - u - v;

	// Interpolate UVs if available
	glm::vec2 uv = glm::vec2(0.0f);
	if (!mesh.UVs.empty()) {
		const glm::vec2& uv0 = mesh.UVs[i0];
		const glm::vec2& uv1 = mesh.UVs[i1];
		const glm::vec2& uv2 = mesh.UVs[i2];
		uv = w * uv0 + u * uv1 + v * uv2;
	}
	payload.UV = uv;

	// Interpolate normals
	glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0)); // default flat normal
	if (!mesh.Normals.empty()) {
		glm::mat4 transform = GetTransform(mesh);
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

		glm::vec3 n0 = glm::normalize(normalMatrix * mesh.Normals[i0]);
		glm::vec3 n1 = glm::normalize(normalMatrix * mesh.Normals[i1]);
		glm::vec3 n2 = glm::normalize(normalMatrix * mesh.Normals[i2]);
		normal = glm::normalize(w * n0 + u * n1 + v * n2);
	}

	// Normal mapping
	glm::vec3 perturbedNormal = normal;
	if (material.Normal.IsValid()) {
		glm::vec3 tangent(1, 0, 0), bitangent(0, 1, 0);

		// Tangent space basis (optional: compute tangents properly)
		if (!mesh.UVs.empty()) {
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;
			glm::vec2 deltaUV1 = mesh.UVs[i1] - mesh.UVs[i0];
			glm::vec2 deltaUV2 = mesh.UVs[i2] - mesh.UVs[i0];

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x + 1e-8f);
			tangent = r * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
			bitangent = glm::normalize(glm::cross(normal, tangent));
			tangent = glm::normalize(tangent);
		}

		glm::mat3 TBN(tangent, bitangent, normal);
		glm::vec3 normalMap = material.Normal.Sample(uv);
		glm::vec3 tangentNormal = normalize(normalMap * 2.0f - 1.0f);

		perturbedNormal = normalize(TBN * tangentNormal);
	}

	payload.WorldNormal = perturbedNormal;
	payload.WorldPosition = position;

	return payload;
}


HitPayload Renderer::ClosestHit(const Ray& ray, float HitDistance, int ObjectIndex)
{
	HitPayload payload;
	payload.HitDistance = HitDistance;
	payload.ObjectIndex = ObjectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[ObjectIndex];

	vec3 origin = ray.Origin - closestSphere.Position;
	vec3 localPosition = origin + HitDistance * ray.Direction;

	vec3 p = normalize(localPosition);
	float u = 0.5f + atan2(p.z, p.x) / (2.0f * glm::pi<float>());
	float v = 0.5f - asin(p.y) / glm::pi<float>();
	payload.UV = vec2(u, v);

	vec3 normalMap = m_ActiveScene->Materials[closestSphere.MaterialIndex].Normal.Sample(payload.UV);
	vec3 tangentNormal = normalize(normalMap * 2.0f - 1.0f);

	payload.WorldPosition = origin + HitDistance * ray.Direction;
	auto worldNormal = normalize(payload.WorldPosition);

	vec3 up = glm::abs(worldNormal.y) < 0.999f ? vec3(0, 1, 0) : vec3(1, 0, 0);
	vec3 tangent = normalize(cross(up, worldNormal));
	vec3 bitangent = cross(worldNormal, tangent);

	mat3 TBN = mat3(tangent, bitangent, worldNormal);
	vec3 perturbedNormal = normalize(TBN * tangentNormal);

	payload.WorldNormal = perturbedNormal;
	payload.WorldPosition += closestSphere.Position; // Convert to world space

	return payload;
}

HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
	payload.HitDistance = -1.0f; // Indicating no hit
	return payload;
}

bool Renderer::IntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v) {
	const float EPSILON = 0.0000001f;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 h = cross(ray.Direction, edge2);
	float a = dot(edge1, h);
	if (abs(a) < EPSILON)
		return false;

	float f = 1.0f / a;
	glm::vec3 s = ray.Origin - v0;
	u = f * dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return false;

	glm::vec3 q = cross(s, edge1);
	v = f * dot(ray.Direction, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;

	t = f * dot(edge2, q);
	return t > EPSILON;
}
