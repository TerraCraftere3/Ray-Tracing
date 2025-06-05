#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "TextureLibrary.h"

struct Material {
	Texture Albedo = TextureDebug();
	Texture Roughness = TextureEmpty();
	Texture Metallic = TextureEmpty();

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionStrength = 0.0f;

	glm::vec3 GetEmission() const {
		return EmissionColor * EmissionStrength;
	}
};

struct Sphere {
	glm::vec3 Position{ 0, 0, 0 };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene {
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};