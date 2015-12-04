#pragma once


#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <memory>


class Backgrounds final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	enum Type
	{
		Sky,
		Hills,
		Trees
	};

	using Ptr = std::unique_ptr<Backgrounds>;


private:
	using SkyContainer = std::array<sf::Sprite, 2>;


public:
							Backgrounds(Type type, const sf::Texture& texture);

	explicit				Backgrounds(Type type, const sf::Texture& texture, const sf::IntRect& rect);

	void					update(float dt, float curve, float moving);


private:
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	Type					mType;
	sf::Sprite				mSprite;
	SkyContainer			mSky;
	float					mSkyTexureWidth;
};