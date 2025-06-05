#pragma once
#include "Texture.h"

#include <vector>

#define CreateTexture(path) TextureLibrary::create(path)
#define CreateColorTexture(r, g, b) TextureLibrary::createColor(glm::vec3(r, g, b))

class TextureLibrary {
public:
	TextureLibrary() = default;
	TextureLibrary(const TextureLibrary&) = delete;

	static TextureLibrary& GetInstance() {
		static TextureLibrary instance;
		return instance;
	}
	static Texture& createColor(glm::vec3 color) {
		auto& instance = GetInstance();
		std::string path = "dynamic/color_" + std::to_string((int)(color.r * 255)) + "_" +
			std::to_string((int)(color.g * 255)) + "_" +
			std::to_string((int)(color.b * 255));
		for (auto& texture : instance.textures) {
			if (texture.GetPath() == path) {
				return texture;
			}
		}
		Texture newTexture(color, path.c_str());
		instance.textures.push_back(newTexture);
		return instance.textures.back();
	}
	static Texture& create(const std::string& path) {
		auto& instance = GetInstance();
		for (auto& texture : instance.textures) {
			if (texture.GetPath() == path) {
				return texture;
			}
		}
		Texture newTexture(path);
		instance.textures.push_back(newTexture);
		return instance.textures.back();
	}

	// --------- Loops ---------- //
	template<typename Func>
	static void ForEachTexture(Func&& func) {
		auto& instance = GetInstance();
		for (auto& texture : instance.textures) {
			func(texture);
		}
	}

	auto begin() { return textures.begin(); }
	auto end() { return textures.end(); }
private:
	std::vector<Texture> textures;
};