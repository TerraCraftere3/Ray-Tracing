#pragma once

#include <chrono>

namespace Utils {
	static double startTime;
	static int frameCount;
	int nextFrame();
	int getFrame();
	double getTime();
	void initTime();
}