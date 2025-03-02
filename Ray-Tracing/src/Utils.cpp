#include "Utils.h"

void Utils::initTime() {
	startTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
	frameCount = 0;
}

int Utils::nextFrame()
{
	frameCount = frameCount + 1;
	return getFrame();
}

int Utils::getFrame()
{
	return frameCount;
}

double Utils::getTime() {
	return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() - startTime;
}