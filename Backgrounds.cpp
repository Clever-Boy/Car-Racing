#include "Backgrounds.hpp"

#include <SFML/Graphics/RenderTarget.hpp>


Backgrounds::Backgrounds(Type type, const sf::Texture& texture)
	: mType(type)
	, mSprite()
	, mSky()
	, mSkyTexureWidth(static_cast<float>(texture.getSize().x))
{
	mSky[0].setTexture(texture);
	mSky[0].setPosition(-mSkyTexureWidth, 0.f);
	mSky[1].setTexture(texture);
}

Backgrounds::Backgrounds(Type type, const sf::Texture& texture, const sf::IntRect& rect)
	: mType(type)
	, mSprite(texture, rect)
	, mSky()
	, mSkyTexureWidth()
{
}

void Backgrounds::update(float dt, float curve, float moving)
{
	if (mType == Type::Sky)
	{
		auto skySpeed = 10.f;

		move(skySpeed * dt, 0.f);

		if (getPosition().x > mSkyTexureWidth)
			move(-mSkyTexureWidth, 0.f);

		return;
	}

	if (moving == 0) return;

	if (mType == Type::Hills)
	{
		auto hillSpeed = 10.f;
		move(-hillSpeed * curve * dt, 0.f);
	}
	else
	{
		auto treeSpeed = 30.f;
		move(-treeSpeed * curve * dt, 0.f);
	}
}

void Backgrounds::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	if (mType == Type::Sky)
	{
		for (const auto& sky : mSky)
			target.draw(sky, states);
	}
	else
	{
		target.draw(mSprite, states);
	}
}