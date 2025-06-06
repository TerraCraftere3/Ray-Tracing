#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "TextureLibrary.h"
#include "string"

struct Material {
	std::string name = "Material";

	Texture Albedo = CreateColorTextureRGB(1, 1, 1);
	Texture Roughness = CreateColorTextureRGB(1, 1, 1);
	Texture Metallic = CreateColorTextureRGB(1, 1, 1);

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionStrength = 0.0f;
	bool isGlass = false;

	glm::vec3 GetEmission() const {
		return EmissionColor * EmissionStrength;
	}
};

struct Sphere {
	std::string name = "Sphere";

	glm::vec3 Position{ 0, 0, 0 };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene {
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};