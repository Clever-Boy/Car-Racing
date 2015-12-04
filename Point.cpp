#include "Point.hpp"

void Point::project(float cameraX, float cameraY, float cameraZ, float cameraDepth, float width, float height, float roadWidth)
{
	camera.x = world.x - cameraX;
	camera.y = world.y - cameraY;
	camera.z = world.z - cameraZ;

	screen.scale = cameraDepth / camera.z;
	screen.x = width / 2u + screen.scale * camera.x  * width / 2u;
	screen.y = height / 2u - screen.scale * camera.y  * height / 2u;
	screen.w = screen.scale * roadWidth * width / 2u;
}