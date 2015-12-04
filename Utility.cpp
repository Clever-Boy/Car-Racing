#include "Utility.hpp"

#include <algorithm>


float utility::increase(float start, float increment, float max)
{
	auto result = start + increment;

	while (result >= max)
		result -= max;

	while (result < 0)
		result += max;

	return result;
}

float utility::limit(float value, float min, float max)
{
	return std::max(min, std::min(value, max));
}

float utility::rumbleWidth(float projectedRoadWidth, std::size_t lanes)
{
	return projectedRoadWidth / std::max(6u, 2 * lanes);
}

float utility::laneMarkerWidth(float projectedRoadWidth, std::size_t lanes)
{
	return projectedRoadWidth / std::max(32u, 8 * lanes);
}

float utility::exponentialFog(float distance, float density)
{
	return static_cast<float>(1 / std::pow(M_E, (distance * distance * density)));
}

float utility::easeIn(float a, float b, float percent)
{
	return a + (b - a) * std::pow(percent, 2.f);
}

float utility::easeOut(float a, float b, float percent)
{
	return a + (b - a) * (1 - std::pow(1.f - percent, 2.f));
}

float utility::easeInOut(float a, float b, float percent)
{
	return a + (b - a) * (static_cast<float>(-std::cos(percent * M_PI) / 2.f) + 0.5f);
}

float utility::interpolate(float a, float b, float percent)
{
	return a + (b - a) * percent;
}

float utility::percentRemaining(float n, float total)
{
	return (static_cast<int>(n) % static_cast<int>(total)) / total;
}