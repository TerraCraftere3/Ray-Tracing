#pragma once

#include <Platform/Image.h>
#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
		bool MultiThreaded = true;
	};
public:
	Renderer() = default;

	void Render(const Camera& camera, const Scene& scene);
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Image> getFinalImage() const { return m_FinalImage; }

	Settings& GetSettings() { return m_Settings; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
private:
	struct HitPayload {
		float HitDistance;
		int ObjectIndex = -1;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;
		glm::vec2 UV;

		int MeshIndex = -1;
		int TriangleIndex = -1;
		float BaryU = 0.0f, BaryV = 0.0f;
	};


	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float HitDistance, int ObjectIndex);
	HitPayload ClosestHit(const Ray& ray, float t, int meshIndex, int triIndex, float u, float v);
	HitPayload Miss(const Ray& ray);
	bool IntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v);
private:
	std::shared_ptr<Image> m_FinalImage;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
	Settings m_Settings;
};