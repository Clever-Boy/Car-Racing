#pragma once


#include "Sprites.hpp"
#include "Cars.hpp"
#include "Point.hpp"
#include "Polygon.hpp"
#include "Colors.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/NonCopyable.hpp>


class Segment final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Segment>;


private:
	using SpritesContainer = std::vector<Sprites::Ptr>;
	using CarsContainer = std::vector<Cars::Ptr>;


public:
	void					setCurve(float i);
	float					getCurve() const;

	Point&					point1();
	Point&					point2();
	const					Point&  point1() const;
	const					Point&  point2() const;

	void					setSegmentColors(Colors::Type color);

	void					setIndex(std::size_t index);
	std::size_t				getIndex() const;

	void					setClip(float clip);
	const float				getClip() const;

	const SpritesContainer& getSprites() const;
	void					addSprite(Sprites::Ptr sprite);

	const CarsContainer&	getCars() const;
	void					removeCar();
	void					addCar(Cars::Ptr car);

	void					setGrounds(float width, float fog);


private:
	void					draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	Point					mPoint1{};
	Point					mPoint2{};
	Polygon					mRumbleSide1{};
	Polygon					mRumbleSide2{};
	Polygon					mLanes1{};
	Polygon					mLanes2{};
	Polygon					mMainRoad{};
	sf::RectangleShape		mLandscape{};
	sf::RectangleShape		mFog{};
	SpritesContainer		mSprites{};
	CarsContainer			mCars{};
	std::size_t				mIndex{};
	Colors					mColors{};
	float					mCurve{};
	float					mClip{};
};