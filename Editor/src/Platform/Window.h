#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/Window.h"
#else
#error "Only Windows Support for Window.h!!!"
#endif