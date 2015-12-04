#include "Sprites.hpp"
#include "ResourceHolder.hpp"
#include "SpritesData.hpp"

#include  <SFML/Graphics/RenderTarget.hpp>


Sprites::Sprites(const TextureHolder& textures, const sf::IntRect& rect, float offset)
	: mSprite(textures.get(Textures::Sprites), rect)
	, mIsDrawing(false)
	, mOffset(offset)
{
}

float Sprites::getOffset() const 
{ 
	return mOffset;
}

sf::FloatRect Sprites::getBoundingRect() const
{
	return getTransform().transformRect(mSprite.getGlobalBounds());
}

void Sprites::update(float width, float roadWidth, float scaleXY, float destX, float destY, float offsetX, float offsetY, float clip)
{
	SpritesData spritesData;

	auto destW = (mSprite.getLocalBounds().width * scaleXY * width / 2) * (spritesData.Scale * roadWidth);
	auto destH = (mSprite.getLocalBounds().height * scaleXY * width / 2) * (spritesData.Scale * roadWidth);

	destX += destW * offsetX;
	destY += destH * offsetY;

	auto clipH = clip != 0.f ? std::max(0.f, destY + destH - clip) : 0.f;

	if (clipH >= destH) return;

	mIsDrawing = true;
	setPosition(destX, destY);
	setScale(destW / mSprite.getLocalBounds().width, (destH - clipH) / mSprite.getLocalBounds().height);
}

void Sprites::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!mIsDrawing) return;

	states.transform *= getTransform();
	target.draw(mSprite, states);

	mIsDrawing = false;
}