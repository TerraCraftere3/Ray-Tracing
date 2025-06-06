#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/Window.h"
#elif defined(PLATFORM_LINUX)
#include "Linux/Window.h"
#endif