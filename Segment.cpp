#include "Segment.hpp"
#include "Utility.hpp"

#include  <SFML/Graphics/RenderTarget.hpp>


void Segment::setCurve(float i)
{ 
	mCurve = i;
}

float Segment::getCurve() const 
{ 
	return mCurve;
}

Point& Segment::point1()
{ 
	return mPoint1; 
}

Point&Segment::point2()
{ 
	return mPoint2;
}

const Point& Segment::point1() const
{ 
	return mPoint1;
}

const Point&  Segment::point2() const
{
	return mPoint2; 
}

void Segment::setSegmentColors(Colors::Type color) 
{ 
	mColors.setType(color); 
}

void Segment::setIndex(std::size_t index)
{
	mIndex = index;
}

std::size_t Segment::getIndex() const 
{ 
	return mIndex; 
}

void Segment::setClip(float clip) 
{
	mClip = clip;
}

const float Segment::getClip() const 
{ 
	return mClip;
}

const Segment::SpritesContainer& Segment::getSprites() const
{ 
	return mSprites; 
}

void Segment::addSprite(Sprites::Ptr sprite)
{
	mSprites.push_back(std::move(sprite));
}

const Segment::CarsContainer& Segment::getCars() const
{ 
	return mCars;
}

void Segment::removeCar()
{ 
	mCars.pop_back();
}

void Segment::addCar(Cars::Ptr car)
{ 
	mCars.push_back(car); 
}

void Segment::setGrounds(float width, float fog)
{
	auto lanes = 3u;

	// Landscape
	mLandscape.setSize({ width, mPoint1.screen.y - mPoint2.screen.y });
	mLandscape.setPosition(0, mPoint2.screen.y);
	mLandscape.setFillColor(mColors.getGrass());

	// Rumble sides
	auto rumbleWidth1 = utility::rumbleWidth(mPoint1.screen.w, lanes);
	auto rumbleWidth2 = utility::rumbleWidth(mPoint2.screen.w, lanes);

	mRumbleSide1.setVertices(
		mPoint1.screen.x - mPoint1.screen.w - rumbleWidth1, mPoint1.screen.y,
		mPoint1.screen.x - mPoint1.screen.w, mPoint1.screen.y,
		mPoint2.screen.x - mPoint2.screen.w, mPoint2.screen.y,
		mPoint2.screen.x - mPoint2.screen.w - rumbleWidth2, mPoint2.screen.y, mColors.getRumble());

	mRumbleSide2.setVertices(
		mPoint1.screen.x + mPoint1.screen.w + rumbleWidth1, mPoint1.screen.y,
		mPoint1.screen.x + mPoint1.screen.w, mPoint1.screen.y,
		mPoint2.screen.x + mPoint2.screen.w, mPoint2.screen.y,
		mPoint2.screen.x + mPoint2.screen.w + rumbleWidth2, mPoint2.screen.y, mColors.getRumble());

	// Main Road
	mMainRoad.setVertices(
		mPoint1.screen.x - mPoint1.screen.w, mPoint1.screen.y,
		mPoint1.screen.x + mPoint1.screen.w, mPoint1.screen.y,
		mPoint2.screen.x + mPoint2.screen.w, mPoint2.screen.y,
		mPoint2.screen.x - mPoint2.screen.w, mPoint2.screen.y, mColors.getRoad());

	// Lanes
	auto laneMarkerWidth1 = utility::laneMarkerWidth(mPoint1.screen.w, lanes);
	auto laneMarkerWidth2 = utility::laneMarkerWidth(mPoint2.screen.w, lanes);
	auto lanew1 = mPoint1.screen.w * 2 / lanes;
	auto lanew2 = mPoint2.screen.w * 2 / lanes;
	auto lanex1 = mPoint1.screen.x - mPoint1.screen.w + lanew1;
	auto lanex2 = mPoint2.screen.x - mPoint2.screen.w + lanew2;

	for (auto lane = 1u; lane < lanes; lanex1 += lanew1 + 1, lanex2 += lanew2 + 1, lane++)
	{
		if (lane == 1)
			mLanes1.setVertices(
				lanex1 - laneMarkerWidth1 / 2, mPoint1.screen.y,
				lanex1 + laneMarkerWidth1 / 2, mPoint1.screen.y,
				lanex2 + laneMarkerWidth2 / 2, mPoint2.screen.y,
				lanex2 - laneMarkerWidth2 / 2, mPoint2.screen.y, mColors.getLane());
		else
			mLanes2.setVertices(
				lanex1 - laneMarkerWidth1 / 2, mPoint1.screen.y,
				lanex1 + laneMarkerWidth1 / 2, mPoint1.screen.y,
				lanex2 + laneMarkerWidth2 / 2, mPoint2.screen.y,
				lanex2 - laneMarkerWidth2 / 2, mPoint2.screen.y, mColors.getLane());
	}

	// Fog effect
	mFog.setSize({ width, mPoint1.screen.y - mPoint2.screen.y });
	mFog.setPosition(0, mPoint2.screen.y);
	mFog.setFillColor(sf::Color(0, 81, 8, 255 - static_cast<unsigned char>(fog * 255)));
}

void Segment::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	target.draw(mLandscape, states);
	target.draw(mRumbleSide1, states);
	target.draw(mRumbleSide2, states);
	target.draw(mMainRoad, states);
	target.draw(mLanes1, states);
	target.draw(mLanes2, states);
	target.draw(mFog, states);
}