#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/Image.h"
#elif defined(PLATFORM_LINUX)
#include "Linux/Image.h"
#endif
