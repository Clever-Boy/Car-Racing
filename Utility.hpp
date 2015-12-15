#pragma once


#define _USE_MATH_DEFINES
#include <cmath>
#include <random>


#ifndef M_PI
#define M_PI 3.141592653589793238462643383
#endif 

#ifndef M_E
#define M_E 2.71828182845904523536
#endif 


namespace utility
{
	float increase(float start, float increment, float max);

	float limit(float value, float min, float max);

	float rumbleWidth(float projectedRoadWidth, std::size_t lanes);

	float laneMarkerWidth(float projectedRoadWidth, std::size_t lanes);

	float exponentialFog(float distance, float density);

	float easeIn(float a, float b, float percent);

	float easeOut(float a, float b, float percent);

	float easeInOut(float a, float b, float percent);

	float interpolate(float a, float b, float percent);

	float percentRemaining(float n, float total);

	template <typename T>
	auto dist() -> typename std::enable_if_t<std::is_integral<T>::value, std::uniform_int_distribution<T>> {}

	template <typename T>
	auto dist() -> typename std::enable_if_t<std::is_floating_point<T>::value, std::uniform_real_distribution<T>> {}

	template<class T = std::mt19937, std::size_t N = T::state_size>
	auto ProperlySeededRandomEngine() -> typename std::enable_if_t<!!N, T>;

	template<typename T>
	T random(T min, T max);

	template<typename C>
	auto randomChoice(const C& constainer);
}

#include "Utility.inl"