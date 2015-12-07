#include "Player.hpp"
#include "ResourceHolder.hpp"
#include "SpritesData.hpp"

#include <SFML/Graphics/RenderTarget.hpp>


Player::Player(const TextureHolder& textures, const sf::IntRect& rect, float z)
	: mSprite(textures.get(Textures::Sprites), rect)
	, mOffset()
	, mZvalue(z)
	, mSpeed()
{
}

void Player::setOffset(float offset)
{
	mOffset = offset;
}

float Player::getOffset() const
{
	return mOffset;
}

void Player::setZValue(float z)
{
	mZvalue = z;
}

float Player::getZValue()const
{
	return mZvalue;
}

void Player::setSpeed(float speed)
{
	mSpeed = speed;
}

float Player::getSpeed() const
{
	return mSpeed;
}

void Player::moveLeft(float amount)
{
	mOffset -= amount;
}

void Player::moveRight(float amount)
{
	mOffset += amount;
}

void Player::adaptMoving(float amount)
{
	mOffset -= amount;
}

void Player::accelerate(float amount)
{
	mSpeed += amount;
}

sf::FloatRect Player::getBoundingRect() const
{
	return getTransform().transformRect(mSprite.getGlobalBounds());
}

void Player::update(float width, float roadWidth, float scaleXY, float destX, float destY, int steer, float updown)
{
	SpritesData spritesData;

	sf::IntRect spriteRect;

	if (steer < 0)
		spriteRect = (updown > 0) ? spritesData.PlayerUpHillLeft : spritesData.PlayerLeft;
	else if (steer > 0)
		spriteRect = (updown > 0) ? spritesData.PlayerUpHillRight : spritesData.PlayerRight;
	else
		spriteRect = (updown > 0) ? spritesData.PlayerUpHillStraight : spritesData.PlayerStraight;

	mSprite.setTextureRect(spriteRect);

	auto destW = (mSprite.getLocalBounds().width * scaleXY * width / 2) * (spritesData.Scale * roadWidth);
	auto destH = (mSprite.getLocalBounds().height * scaleXY * width / 2) * (spritesData.Scale * roadWidth);

	destX += destW * -0.5f;
	destY += destH * -1;

	setPosition(destX, destY);
	setScale(destW / mSprite.getLocalBounds().width, destH / mSprite.getLocalBounds().height);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(mSprite, states);
}