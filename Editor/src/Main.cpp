#include "Core/Core.h"
#include "Core/Log.h"

#include "Platform/Window.h"
#include "Platform/ImGui.h"
#include "Platform/Input.h"

#include "Rendering/Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <string>
#include <filesystem>
#include <set>
#include <optional>
#include <unordered_map>

void ShowTextureLibrary()
{
	static std::optional<std::string> selectedTexturePath;

	const float thumbnailSize = 64.0f;
	const float padding = 8.0f;
	const float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	// We'll split the panel 60% grid / 40% details
	float gridPanelWidth = panelWidth * 0.6f;
	float detailPanelWidth = panelWidth - gridPanelWidth;

	// Begin a child window for the grid on left
	ImGui::BeginChild("TextureGrid", ImVec2(gridPanelWidth, 0), true);

	int index = 0;
	int columnCount = std::max(1, (int)(gridPanelWidth / cellSize));

	TextureLibrary::ForEachTexture([&](Texture& tex) {
		if (index % columnCount != 0)
			ImGui::SameLine();

		ImGui::BeginGroup();

		std::string fullPath = tex.GetPath();
		std::string filename = std::filesystem::path(fullPath).filename().string();

		ImVec2 textSize = ImGui::CalcTextSize(filename.c_str());
		float cellWidth = std::max(thumbnailSize, textSize.x);
		float cursorX = ImGui::GetCursorPosX();
		float centerOffset = (cellWidth - thumbnailSize) * 0.5f;

		//ImGui::SetCursorPosX(cursorX + centerOffset);
		if (tex.IsValid()) {
			ImGui::Image((ImTextureID)(uintptr_t)tex.GetID(), ImVec2(thumbnailSize, thumbnailSize));
		}
		else {
			ImGui::Dummy(ImVec2(thumbnailSize, thumbnailSize));
		}

		// Click selects the texture for detail view
		if (ImGui::IsItemClicked()) {
			if (selectedTexturePath && *selectedTexturePath == fullPath)
				selectedTexturePath.reset(); // deselect on 2nd click
			else
				selectedTexturePath = fullPath;
		}

		// Draw filename centered below
		//float textOffset = (cellWidth - textSize.x) * 0.5f;
		//ImGui::SetCursorPosX(cursorX + textOffset);
		//ImGui::TextWrapped("%s", filename.c_str());

		ImGui::EndGroup();
		++index;
		});

	ImGui::EndChild();

	ImGui::SameLine();

	// Begin child window for details on right
	ImGui::BeginChild("TextureDetails", ImVec2(detailPanelWidth, 0), true);

	if (selectedTexturePath) {
		// Find the texture by path
		Texture* selectedTex = nullptr;
		TextureLibrary::ForEachTexture([&](Texture& tex) {
			if (tex.GetPath() == *selectedTexturePath) {
				selectedTex = &tex;
			}
			});

		if (selectedTex) {
			ImGui::Text("Filename: %s", std::filesystem::path(selectedTex->GetPath()).filename().string().c_str());
			ImGui::Text("Full Path: %s", selectedTex->GetPath().c_str());
			ImGui::Text("Size: %dx%d", selectedTex->GetWidth(), selectedTex->GetHeight());
			ImGui::Text("OpenGL ID: %u", selectedTex->GetID());

			if (selectedTex->IsValid()) {
				ImGui::Image((ImTextureID)(uintptr_t)selectedTex->GetID(), ImVec2(256, 256));
			}
			else {
				ImGui::TextColored(ImVec4(1, 0, 1, 1), "Texture data not valid");
			}
		}
		else {
			ImGui::Text("Selected texture not found.");
		}
	}
	else {
		ImGui::Text("Select a texture to see details.");
	}

	ImGui::EndChild();
}

void DisplayImage(const Texture& texture, const std::string& label)
{
	ImTextureID myImage = texture.GetID();
	ImVec2 imageSize = ImVec2(256, 256);

	static std::unordered_map<ImGuiID, bool> showImageMap;

	// Push a unique ID to separate this UI block
	ImGui::PushID(label.c_str()); // Ensures uniqueness per call
	ImGui::Text("%s: %s", label.c_str(), texture.GetPath().c_str());
	ImGuiID id = ImGui::GetID("image_toggle"); // Unique ID inside pushed context

	if (ImGui::IsItemClicked()) {
		showImageMap[id] = !showImageMap[id];
	}

	if (showImageMap[id]) {
		ImGui::Image(myImage, imageSize);
	}
	ImGui::PopID();
}

int main()
{
    Logger::Init();
    Window window("RayTracer", 1600, 1000);
	UIManager uiManager(&window);
	Input::Init(window);
	LOG_INFO("Application started");

	Renderer renderer;
	Scene scene;
	Camera camera(45.0f, 0.1f, 100.0f);

	float viewportWidth = 0.0f;
	float viewportHeight = 0.0f;

	using Clock = std::chrono::high_resolution_clock;
	auto lastTime = Clock::now();
	float deltaTime = 0.0f;
	float frameTime = 0.0f;
	float fps = 0.0f;

	{
		{
			Sphere s;
			s.Position = { 0, 0, 0 };
			s.Radius = 1.0f;
			s.MaterialIndex = 0;
			scene.Spheres.push_back(s);
		}
		{
			Sphere s;
			s.Position = { 2, 0, 0 };
			s.Radius = 1.0f;
			s.MaterialIndex = 2;
			scene.Spheres.push_back(s);
		}
		{
			Sphere s;
			s.Position = { 0, -101, 0 };
			s.Radius = 100.0f;
			s.MaterialIndex = 1;
			scene.Spheres.push_back(s);
		}
		{
			Material m;
			m.Albedo = CreateTexture("textures/rock/rock-wall-mortar_albedo.png");
			m.Roughness = CreateTexture("textures/rock/rock-wall-mortar_roughness.png");;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.Albedo = CreateTexture("textures/plaster/rough-plaster-basecolor.png");
			m.Roughness = CreateTexture("textures/plaster/rough-plaster-roughness.png");;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.EmissionColor = { 0.8f, 0.5f, 0.2f };
			m.EmissionStrength = 2;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.isGlass = true;
			m.Albedo = CreateColorTexture(1, 1, 1);
			scene.Materials.push_back(m);
		}
	}

#define IMGUI_CONTROL_WITH_RESET(Func) \
	if (Func) renderer.ResetFrameIndex()

	while (!window.shouldClose())
	{
		window.beginFrame();
		uiManager.beginFrame();

		ImGui::Begin("Settings");
		ImGui::Text("FPS: %.1f", fps);
		ImGui::Text("Frame Time: %.2f ms", frameTime);
		ImGui::Checkbox("Multi-threaded", &renderer.GetSettings().MultiThreaded);
		ImGui::Checkbox("Accumulate", &renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset")) {
			renderer.ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Text("Spheres: %d", (int)scene.Spheres.size());
		for (size_t i = 0; i < scene.Spheres.size(); i++) {
			auto& sphere = scene.Spheres[i];
			ImGui::PushID(i);
			IMGUI_CONTROL_WITH_RESET(ImGui::DragFloat3("Position",glm::value_ptr(sphere.Position), 0.01f));
			IMGUI_CONTROL_WITH_RESET(ImGui::DragFloat("Radius", &sphere.Radius, 0.01f, 0.01f, 10.0f));
			IMGUI_CONTROL_WITH_RESET(ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)scene.Materials.size() - 1));
			if (ImGui::Button("Remove")) {
				renderer.ResetFrameIndex();
				scene.Spheres.erase(scene.Spheres.begin() + i);
				i--; // Adjust index after removal
			}
			ImGui::PopID();
			ImGui::Separator();
		}
		if (ImGui::Button("Add Sphere")) {
			renderer.ResetFrameIndex();
			scene.Spheres.push_back(Sphere());
		}
		ImGui::End();

		ImGui::Begin("Materials");
		ImGui::Text("Materials: %d", (int)scene.Materials.size());
		for (size_t i = 0; i < scene.Materials.size(); i++) {
			auto& material = scene.Materials[i];
			ImGui::PushID(i);

			DisplayImage(material.Albedo, "Albedo");
			DisplayImage(material.Roughness, "Roughness");
			DisplayImage(material.Metallic, "Metallic");

			IMGUI_CONTROL_WITH_RESET(ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor)));
			IMGUI_CONTROL_WITH_RESET(ImGui::DragFloat("Emission Strength", &material.EmissionStrength, 0.01f, 0.0f, FLT_MAX));
			if (ImGui::Button("Remove")) {
				renderer.ResetFrameIndex();
				scene.Materials.erase(scene.Materials.begin() + i);
				i--; // Adjust index after removal
			}
			ImGui::PopID();
			ImGui::Separator();
		}
		if (ImGui::Button("Add Material")) {
			renderer.ResetFrameIndex();
			scene.Materials.push_back(Material());
		}
		ImGui::End();

		ImGui::Begin("Textures");
		ShowTextureLibrary();
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;
		auto image = renderer.getFinalImage();
		if (image)
			ImGui::Image((ImTextureID)(intptr_t)image->GetTextureID(), ImVec2((float)image->GetWidth(), (float)image->GetHeight()));

		ImGui::End();
		ImGui::PopStyleVar();

		auto startTime = Clock::now();
		camera.OnResize(viewportWidth, viewportHeight);
		renderer.OnResize(viewportWidth, viewportHeight);
		renderer.Render(camera, scene);
		auto endTime = Clock::now();
		deltaTime = std::chrono::duration<float>(endTime - lastTime).count();
		frameTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
		fps = 1.0f / deltaTime;
		lastTime = endTime;

		if(camera.OnUpdate(deltaTime))
			renderer.ResetFrameIndex();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uiManager.endFrame();
		window.endFrame();
	}

	window.shutdown();
    return 0;
}
