#include "Core.h"

void Utils::Empty()
{
}

template<typename First, typename ...Object>
void Utils::Empty(First arg, const Object & ...rest)
{
	Empty(rest...);
}