#pragma once
#include "Platform.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/Input.h"
#else
#error "Only Windows Support for Input.h!!!"
#endif