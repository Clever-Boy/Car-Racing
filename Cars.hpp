#pragma once


#include "ResourceIdentifiers.hpp"
#include "SpritesData.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <memory>


class Cars final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::shared_ptr<Cars>;


public:
	explicit				Cars(const TextureHolder& textures, const sf::IntRect& rect, float offset, float z, float speed);

	void					setPercent(float percent);
	float					getPercent() const;

	void					setOffset(float offset);
	float					getOffset() const;

	void					setZValue(float z);
	float					getZValue() const;

	float					getSpeed() const;

	sf::FloatRect			getBoundingRect() const;

	void					update(float width, float roadWidth, float scaleXY, float destX, float destY, float offsetX, float offsetY, float clip);


private:
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	sf::Sprite				mSprite;
	SpritesData				mSpritesData;
	mutable bool			mIsDrawing;
	float					mOffset;
	float					mZvalue;
	float					mSpeed;
	float					mPercent;
};