#include "Polygon.hpp"

#include <SFML/Graphics/RenderTarget.hpp>


Polygon::Polygon() 
	: mVertices(sf::Quads, 4u)
{
}

void Polygon::setVertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, sf::Color color)
{
	mVertices[0].position = { x1, y1 };
	mVertices[1].position = { x2, y2 };
	mVertices[2].position = { x3, y3 };
	mVertices[3].position = { x4, y4 };

	mVertices[0].color = mVertices[1].color = mVertices[2].color = mVertices[3].color = color;
}


void Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(mVertices, states);
}