#pragma once


#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/NonCopyable.hpp>


#include <memory>


class Player final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Player>;


public:
	explicit				Player(const TextureHolder& textures, const sf::IntRect& rect, float z);

	void					setOffset(float offset);
	float					getOffset() const;

	void					setZValue(float z);
	float					getZValue() const;

	void					setSpeed(float speed);
	float					getSpeed() const;

	void					moveLeft(float amount);
	void					moveRight(float amount);
	void					adaptMoving(float amount);
	void					accelerate(float amount);

	sf::FloatRect			getBoundingRect() const;

	void					update(float width, float roadWidth, float scaleXY, float destX, float destY, int steer, float updown);


private:
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	sf::Sprite				mSprite;
	float					mOffset;
	float					mZvalue;
	float					mSpeed;
};