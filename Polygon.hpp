#pragma once


#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/NonCopyable.hpp>


class Polygon final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
							Polygon();

	void					setVertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, sf::Color color);


private:
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	sf::VertexArray			mVertices;
};