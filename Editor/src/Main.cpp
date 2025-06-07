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
#include <Platform/Models.h>
#include <Rendering/CPURenderer.h>
#include <Rendering/GPURenderer.h>

bool ShowMaterial(Material& mat)
{
	bool edited = false;

	if (ImGui::TreeNodeEx(mat.name.c_str()))
	{
		char nameBuffer[128];
		strncpy(nameBuffer, mat.name.c_str(), sizeof(nameBuffer));
		nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
			mat.name = nameBuffer; // Update the original string
		}

		{
			char pathBuffer[256];
			strncpy(pathBuffer, mat.Albedo.GetPath().c_str(), sizeof(pathBuffer));
			pathBuffer[sizeof(pathBuffer) - 1] = '\0'; // Ensure null termination
			if (ImGui::InputText("Albedo Texture", pathBuffer, sizeof(pathBuffer))) {
				edited = true;
				mat.Albedo.LoadTexture(pathBuffer);
			}
		}
		{
			char pathBuffer[256];
			strncpy(pathBuffer, mat.Normal.GetPath().c_str(), sizeof(pathBuffer));
			pathBuffer[sizeof(pathBuffer) - 1] = '\0'; // Ensure null termination
			if (ImGui::InputText("Normal Texture", pathBuffer, sizeof(pathBuffer))) {
				edited = true;
				mat.Normal.LoadTexture(pathBuffer);
			}
		}
		{
			char pathBuffer[256];
			strncpy(pathBuffer, mat.Metallic.GetPath().c_str(), sizeof(pathBuffer));
			pathBuffer[sizeof(pathBuffer) - 1] = '\0'; // Ensure null termination
			if (ImGui::InputText("Metallic Texture", pathBuffer, sizeof(pathBuffer))) {
				edited = true;
				mat.Metallic.LoadTexture(pathBuffer);
			}
		}
		{
			char pathBuffer[256];
			strncpy(pathBuffer, mat.Roughness.GetPath().c_str(), sizeof(pathBuffer));
			pathBuffer[sizeof(pathBuffer) - 1] = '\0'; // Ensure null termination
			if (ImGui::InputText("Roughness Texture", pathBuffer, sizeof(pathBuffer))) {
				edited = true;
				mat.Roughness.LoadTexture(pathBuffer);
			}
		}

		if (ImGui::ColorEdit3("Emission Color", glm::value_ptr(mat.EmissionColor))) edited = true;
		if (ImGui::InputFloat("Emission Strength", &mat.EmissionStrength)) edited = true;

		ImGui::TreePop();
	}

	return edited;
}

bool ShowSphere(Sphere& sphere, Material& mat, Scene* scene)
{
	bool edited = false;

	if (ImGui::TreeNodeEx(sphere.name.c_str()))
	{
		char nameBuffer[128];
		strncpy(nameBuffer, sphere.name.c_str(), sizeof(nameBuffer));
		nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
			sphere.name = nameBuffer; // Update the original string
		}

		if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position))) edited = true;
		if (ImGui::DragFloat("Radius", &sphere.Radius)) edited = true;
		if (ImGui::InputInt("Material Index", &sphere.MaterialIndex)) edited = true;
		if (sphere.MaterialIndex < 0) sphere.MaterialIndex = 0;
		if (sphere.MaterialIndex >= scene->Materials.size()) {
			sphere.MaterialIndex = static_cast<int>(scene->Materials.size()) - 1;
		}

		ShowMaterial(mat);

		ImGui::TreePop();
	}

	return edited;
}

bool ShowMesh(Mesh& mesh, Material& mat, Scene* scene)
{
	bool edited = false;

	if (ImGui::TreeNodeEx(mesh.name.c_str()))
	{
		char nameBuffer[128];
		strncpy(nameBuffer, mesh.name.c_str(), sizeof(nameBuffer));
		nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
			mesh.name = nameBuffer; // Update the original string
		}

		if (ImGui::DragFloat3("Position", glm::value_ptr(mesh.Position))) edited = true;
		if (ImGui::DragFloat3("Scale", glm::value_ptr(mesh.Scale))) edited = true;
		if (ImGui::DragFloat3("Rotation", glm::value_ptr(mesh.Rotation))) edited = true;
		if (ImGui::InputInt("Material Index", &mesh.MaterialIndex)) edited = true;
		if (mesh.MaterialIndex < 0) mesh.MaterialIndex = 0;
		if (mesh.MaterialIndex >= scene->Materials.size()) {
			mesh.MaterialIndex = static_cast<int>(scene->Materials.size()) - 1;
		}

		ShowMaterial(mat);

		ImGui::TreePop();
	}

	return edited;
}

bool ShowScene(Scene* scene, const Renderer* renderer)
{
	bool edited = false;

	if (!scene)
	{
		ImGui::Text("No scene loaded.");
		return false;
	}

	ImGui::Text("Scene Information:");
	ImGui::Text("Spheres: %zu", scene->Spheres.size());
	ImGui::Text("Materials: %zu", scene->Materials.size());

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::TreeNodeEx("Root", flag))
	{
		if (ImGui::TreeNodeEx("Spheres", flag))
		{
			for (int i = 0; i < scene->Spheres.size(); i++)
			{
				Sphere& sphere = scene->Spheres[i]; // Use reference to modify original
				Material& mat = scene->Materials[sphere.MaterialIndex];
				ImGui::PushID(i);

				if (ShowSphere(sphere, mat, scene))
					edited = true;

				ImGui::PopID();
			}

			if (ImGui::Button("Add Sphere"))
			{
				scene->Spheres.push_back(Sphere());
				scene->Spheres.back().MaterialIndex = 0; // Default to first material
				edited = true;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Meshes", flag))
		{
			for (int i = 0; i < scene->Meshes.size(); i++)
			{
				Mesh& mesh = scene->Meshes[i]; // Use reference to modify original
				Material& mat = scene->Materials[mesh.MaterialIndex];
				ImGui::PushID(i);

				if (ShowMesh(mesh, mat, scene))
					edited = true;

				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Materials", flag))
		{
			for (int i = 0; i < scene->Spheres.size(); i++)
			{
				Material& mat = scene->Materials[i]; // Use reference to modify original
				ImGui::PushID(i);

				if (ShowMaterial(mat))
					edited = true;

				ImGui::PopID();
			}

			if (ImGui::Button("Add Material"))
			{
				scene->Materials.push_back(Material());
				edited = true;
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	return edited;
}


void ShowTextureLibrary()
{
	static std::optional<std::string> selectedTexturePath;

	const float thumbnailSize = 100.0f;
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

int main()
{
    Logger::Init();
    Window window("RayTracer", 1600, 1000);
	UIManager uiManager(&window);
	Input::Init(window);
	LOG_INFO("Application started");

	CPURenderer cpuRenderer;
	GPURenderer gpuRenderer; 
	Renderer* renderer = &gpuRenderer; 

	static int currentRendererIndex = 1; // 0 = CPU, 1 = GPU
	const char* rendererOptions[] = { "CPU Renderer", "GPU Renderer" };

	Scene scene;
	Camera camera(45.0f, 0.1f, 100.0f);
	

	float viewportWidth = 0.0f;
	float viewportHeight = 0.0f;

	using Clock = std::chrono::high_resolution_clock;
	auto lastTime = Clock::now();
	float deltaTime = 0.0f;
	float frameTime = 0.0f;
	float fps = 0.0f;
	float renderSize = 0.5f;

	{
		{
			Sphere s;
			s.name = "Small Blue";
			s.Position = { -2, 0, 0 };
			s.Radius = 1.0f;
			s.MaterialIndex = 0;
			scene.Spheres.push_back(s);
		}
		{
			Sphere s;
			s.name = "Light";
			s.Position = { 2, 0, 0 };
			s.Radius = 1.0f;
			s.MaterialIndex = 2;
			scene.Spheres.push_back(s);
		}
		{
			Sphere s;
			s.name = "Floor";
			s.Position = { 0, -101, 0 };
			s.Radius = 100.0f;
			s.MaterialIndex = 1;
			scene.Spheres.push_back(s);
		}
		{
			Mesh suzanneMesh;
			if (LoadOBJMesh("models/suzanne.obj", suzanneMesh)) {
				suzanneMesh.name = "Suzanne";
				suzanneMesh.MaterialIndex = 3;
				suzanneMesh.Scale = { 0.5f, 0.5f, 0.5f };
				scene.Meshes.push_back(suzanneMesh);
			}
			else {
				LOG_ERROR("Failed to load Suzanne mesh.");
			}
		}
		{
			Material m;
			m.name = "Rock Wall";
			m.Albedo = CreateTexture("textures/rock/rock-wall-mortar_albedo.png");
			m.Roughness = CreateTexture("textures/rock/rock-wall-mortar_roughness.png");
			m.Metallic = CreateTexture("textures/rock/rock-wall-mortar_metallic.png");
			m.Normal = CreateTexture("textures/rock/rock-wall-mortar_normal-ogl.png");
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.name = "Rough Plaster";
			m.Albedo = CreateTexture("textures/plaster/rough-plaster-basecolor.png");
			m.Roughness = CreateTexture("textures/plaster/rough-plaster-roughness.png");
			m.Metallic = CreateTexture("textures/plaster/rough-plaster-metallic.png");	
			m.Normal = CreateTexture("textures/plaster/rough-plaster-normal-ogl.png");	
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.name = "Light";
			m.EmissionColor = { 0.8f, 0.5f, 0.2f };
			m.EmissionStrength = 2;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.name = "Gold";
			m.Albedo = CreateTexture("textures/gold/lightgold_albedo.png");
			m.Roughness = CreateTexture("textures/gold/lightgold_roughness.png");
			m.Metallic = CreateTexture("textures/gold/lightgold_metallic.png");
			m.Normal = CreateTexture("textures/gold/lightgold_normal-ogl.png");
			scene.Materials.push_back(m);
		}
	}

#define IMGUI_CONTROL_WITH_RESET(Func) \
	if (Func) renderer->ResetFrameIndex()

#define IMGUI_CONTROL_WITH_RESET_WITH_CALLBACK(Func, Callback) \
	if (Func) { renderer->ResetFrameIndex(); Callback; }

	while (!window.shouldClose())
	{
		window.beginFrame();
		uiManager.beginFrame();

		ImGui::Begin("Settings");
		ImGui::Text("FPS: %.1f", fps);
		ImGui::Text("Frame Time: %.2f ms", frameTime);
		ImGui::Checkbox("Multi-threaded", &renderer->GetSettings().MultiThreaded);
		ImGui::Checkbox("Accumulate", &renderer->GetSettings().Accumulate);
		IMGUI_CONTROL_WITH_RESET_WITH_CALLBACK(ImGui::DragFloat("Render Size", &renderSize, 0.01f, 0.1f, 2.0f, "%.2f"), renderSize = glm::clamp(renderSize, 0.1f, 2.0f));
		if (ImGui::Combo("Renderer", &currentRendererIndex, rendererOptions, IM_ARRAYSIZE(rendererOptions))) {
			if (currentRendererIndex == 0)
				renderer = &cpuRenderer;
			else
				renderer = &gpuRenderer;
		}
		if (ImGui::Button("Reset")) {
			renderer->ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		IMGUI_CONTROL_WITH_RESET(ShowScene(&scene, renderer));
		ImGui::End();


		ImGui::Begin("Textures");
		ShowTextureLibrary();
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;
		if(renderer->isValidImage())
			ImGui::Image((ImTextureID)(intptr_t)renderer->GetFinalImageID(), ImVec2((float)viewportWidth, (float)viewportHeight));

		ImGui::End();
		ImGui::PopStyleVar();

		auto startTime = Clock::now();
		int newWidth = (int)(viewportWidth * renderSize);
		int newHeight = (int)(viewportHeight * renderSize);
		camera.OnResize(newWidth, newHeight);
		renderer->OnResize(newWidth, newHeight);
		renderer->Render(camera, scene);
		auto endTime = Clock::now();
		deltaTime = std::chrono::duration<float>(endTime - lastTime).count();
		frameTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
		fps = 1.0f / deltaTime;
		lastTime = endTime;

		IMGUI_CONTROL_WITH_RESET(camera.OnUpdate(deltaTime));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uiManager.endFrame();
		window.endFrame();
	}

	window.shutdown();
    return 0;
}
