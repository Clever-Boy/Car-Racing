#include "Cars.hpp"
#include "ResourceHolder.hpp"

#include  <SFML/Graphics/RenderTarget.hpp>


Cars::Cars(const TextureHolder& textures, const sf::IntRect& rect, float offset, float z, float speed)
	: mSprite(textures.get(Textures::Sprites), rect)
	, mSpritesData()
	, mIsDrawing(false)
	, mOffset(offset)
	, mZvalue(z)
	, mSpeed(speed)
	, mPercent()
{
}

void Cars::setPercent(float percent)
{ 
	mPercent = percent;
}

float Cars::getPercent() const
{
	return mPercent;
}

void Cars::setOffset(float offset)
{ 
	mOffset = offset; 
}

float Cars::getOffset() const 
{ 
	return mOffset; 
}

void Cars::setZValue(float z)
{
	mZvalue = z; 
}

float Cars::getZValue()const 
{ 
	return mZvalue;
}

float Cars::getSpeed() const
{ 
	return mSpeed;
}

sf::FloatRect Cars::getBoundingRect() const
{
	return getTransform().transformRect(mSprite.getGlobalBounds());
}

void Cars::update(float width, float roadWidth, float scaleXY, float destX, float destY, float offsetX, float offsetY, float clip)
{
	auto destW = (mSprite.getLocalBounds().width * scaleXY * width / 2) * (mSpritesData.Scale * roadWidth);
	auto destH = (mSprite.getLocalBounds().height * scaleXY * width / 2) * (mSpritesData.Scale * roadWidth);

	destX += destW * offsetX;
	destY += destH * offsetY;

	auto clipH = clip != 0.f ? std::max(0.f, destY + destH - clip) : 0.f;

	if (clipH >= destH) return;

	mIsDrawing = true;

	auto flip = mOffset <= 0.5f ? 1 : -1;

	auto destScaleX = flip * destW / mSprite.getLocalBounds().width;
	auto destScaleY = (destH - clipH) / mSprite.getLocalBounds().height;

	setPosition(destX, destY);
	setScale(destScaleX, destScaleY);
}

void Cars::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!mIsDrawing) return;

	states.transform *= getTransform();
	target.draw(mSprite, states);

	mIsDrawing = false;
}