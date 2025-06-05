#pragma once
#include "Texture.h"

#include <vector>

#define CreateTexture(path) TextureLibrary::create(path)
#define TextureEmpty() TextureLibrary::GetInstance().create("textures/none.png")
#define TextureDebug() TextureLibrary::GetInstance().create("textures/debug.png")

class TextureLibrary {
public:
	TextureLibrary() = default;
	TextureLibrary(const TextureLibrary&) = delete;

	static TextureLibrary& GetInstance() {
		static TextureLibrary instance;
		return instance;
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