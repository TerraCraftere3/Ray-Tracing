#include "Core/Log.h"

#include "Platform/Window.h"
#include "Platform/ImGui.h"

int main()
{
    Logger::Init();
    Window window("RayTracer");
	UIManager uiManager(&window);
	LOG_INFO("Application started");

	while (!window.shouldClose())
	{
		window.beginFrame();
		uiManager.beginFrame();

		ImGui::Begin("Hello, RayTracer!");
		ImGui::Text("Hi!");
		ImGui::End();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uiManager.endFrame();
		window.endFrame();
	}

	window.shutdown();
    return 0;
}
