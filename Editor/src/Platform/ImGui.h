#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/ImGuiManager.h"
#else
#error "Only Windows Support for ImGui.h!!!"
#endif