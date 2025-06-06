#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/Input.h"
#elif defined(PLATFORM_LINUX)
#include "Linux/Input.h"
#endif