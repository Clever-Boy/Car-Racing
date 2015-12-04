#pragma once


#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Backgrounds.hpp"
#include "Player.hpp"
#include "Segment.hpp"

#include <SFML/Graphics/Texture.hpp>


namespace sf
{
	class RenderWindow;
}


class World final :  private sf::NonCopyable
{
private:
	struct Length
	{
		const float none = 0.f;
		const float shorty = 25.f;
		const float medium = 50.f;
		const float longy = 100.f;
	} length;

	struct Curve
	{
		const float none = 0.f;
		const float easy = 2.f;
		const float medium = 4.f;
		const float hard = 6.f;
	} curve;

	struct Hill
	{
		const float none = 0.f;
		const float low = 20.f;
		const float medium = 40.f;
		const float high = 60.f;
	} hill;

	using SegmentContainer = std::vector<Segment::Ptr>;
	using BackgroundContainer = std::vector<Backgrounds::Ptr>;
	using CarsContainer = std::vector<Cars::Ptr>;


public:
	explicit				World(sf::RenderWindow& window);

	void					update(float dt);
	void					draw();


private:
	void					loadTextures();
	void					buildScene();
	void					addSegment(float curve, float y);
	void					addSprite(std::size_t n, const sf::IntRect& sprite, float offset);
	void					addRoad(float enter, float hold, float leave, float curve, float y);
	void					addStraight(float n = 0);
	void					addHill(float n = 0, float h = 0);
	void					addCurve(float n = 0, float c = 0, float h = 0);
	void					addDownhillToEnd(float n = 0);
	void					addSCurves();
	void					addLowRollingHills(float n = 0, float h = 0);
	void					addBumps();
	float					lastY();


private:
	sf::RenderWindow&		mWindow;
	TextureHolder			mTextures;
	SegmentContainer		mSegments;
	BackgroundContainer		mBackground;
	std::size_t				mRumbleLength;
	CarsContainer			mCars;
	Player::Ptr				mPlayer;
	float					mSegmentLength;
	float					mPlayerX;
	float					mCameraDepth;
	float					mCameraHeight;
	float					mPlayerZ;
	float					mPosition;
	float					mTrackLength;
	float					mSpeed;
	int						mSteer;
	float					mMaxSpeed;
};