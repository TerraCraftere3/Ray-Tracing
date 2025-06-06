#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/ImGuiManager.h"
#elif defined(PLATFORM_LINUX)
#include "Linux/ImGuiManager.h"
#endif