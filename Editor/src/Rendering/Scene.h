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
	Texture Normal = CreateColorTextureRGB(0.5f, 0.5f, 1.0f);

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionStrength = 0.0f;
	bool isGlass = false;

	glm::vec3 GetEmission() const {
		return EmissionColor * EmissionStrength;
	}
};

struct Mesh {
	std::string name = "Mesh";

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UVs;
	std::vector<uint32_t> Indices;

	glm::vec3 Position{ 0, 0, 0 };
	glm::vec3 Rotation{ 0, 0, 0 };
	glm::vec3 Scale{ 1, 1, 1 };

	int MaterialIndex = 0;
};

struct Sphere {
	std::string name = "Sphere";

	glm::vec3 Position{ 0, 0, 0 };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene {
	std::vector<Sphere> Spheres;
	std::vector<Mesh> Meshes;
	std::vector<Material> Materials;
};

struct GPU_Material {
	glm::vec3 albedo;
	glm::vec3 emission;
	float roughness;
	float padding1;
};

struct GPU_Sphere {
	glm::vec3 position;
	float radius;
	int materialIndex;
	glm::vec3 padding; // pad to 32 bytes
};