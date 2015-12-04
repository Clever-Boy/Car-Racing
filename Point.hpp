#pragma once

#include <SFML/System/Vector3.hpp>

struct Point
{
	struct Screen
	{
		float x{};
		float y{};
		float w{};
		float scale{};

	} screen{};

	sf::Vector3f world{};
	sf::Vector3f camera{};

	void project(float cameraX, float cameraY, float cameraZ, float cameraDepth, float width, float height, float roadWidth);
};