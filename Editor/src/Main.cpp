#include "Core/Core.h"
#include "Core/Log.h"

#include "Platform/Window.h"
#include "Platform/ImGui.h"
#include "Platform/Input.h"

#include "Rendering/Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <string>

int main()
{
    Logger::Init();
    Window window("RayTracer", 1400, 900);
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
			m.Albedo = { 1.0f, 0.0f, 1.0f };
			m.Roughness = 1.0f;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.Albedo = { 0.2f, 0.3f, 1.0f };
			m.Roughness = 0.1f;
			scene.Materials.push_back(m);
		}
		{
			Material m;
			m.Albedo = { 0.8f, 0.5f, 0.2f };
			m.Roughness = 0.1f;
			m.EmissionColor = m.Albedo;
			m.EmissionStrength = 2;
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
			IMGUI_CONTROL_WITH_RESET(ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo)));
			IMGUI_CONTROL_WITH_RESET(ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f));
			IMGUI_CONTROL_WITH_RESET(ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f));
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
