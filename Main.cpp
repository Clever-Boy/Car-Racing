#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <random>
#include <stdexcept>
#include <cassert>
#include <iostream>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383
#endif 

#ifndef M_E
#define M_E 2.71828182845904523536
#endif 

namespace
{
	float increase(float start, float increment, float max)
	{
		auto result = start + increment;

		while (result >= max)
			result -= max;

		while (result < 0)
			result += max;

		return result;
	}

	float limit(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));
	}

	float rumbleWidth(float projectedRoadWidth, std::size_t lanes)
	{
		return projectedRoadWidth / std::max(6u, 2 * lanes);
	}

	float laneMarkerWidth(float projectedRoadWidth, std::size_t lanes)
	{
		return projectedRoadWidth / std::max(32u, 8 * lanes);
	}

	float exponentialFog(float distance, float density)
	{
		return static_cast<float>(1 / std::pow(M_E, (distance * distance * density)));
	}

	float easeIn(float a, float b, float percent)
	{
		return a + (b - a) * std::pow(percent, 2.f);
	}

	float easeOut(float a, float b, float percent)
	{
		return a + (b - a) * (1 - std::pow(1.f - percent, 2.f));
	}

	float easeInOut(float a, float b, float percent)
	{
		return a + (b - a) * (static_cast<float>(-std::cos(percent * M_PI) / 2.f) + 0.5f);
	}

	float interpolate(float a, float b, float percent)
	{
		return a + (b - a) * percent;
	}

	float percentRemaining(float n, float total)
	{
		return (static_cast<int>(n) % static_cast<int>(total)) / total;
	}

	template <typename U>
	static auto dist() -> typename std::enable_if_t<std::is_integral<U>::value, std::uniform_int_distribution<U>> {}

	template <typename U>
	static auto dist() -> typename std::enable_if_t<std::is_floating_point<U>::value, std::uniform_real_distribution<U>> {}

	template<typename U>
	auto random(U min, U max)
	{
		static auto seed = static_cast<unsigned long>(std::time(nullptr));
		static auto RandomEngine = std::default_random_engine(seed);

		decltype(dist<U>()) uniformDistribution(min, max);

		return uniformDistribution(RandomEngine);
	}

	template<typename C>
	auto randomChoice(const C& constainer)
	{
		return constainer[random(0u, constainer.size() - 1)];
	}
}

class Polygon final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	Polygon() : mVertices(sf::Quads, 4u) {}

	void setVertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, sf::Color color)
	{
		mVertices[0].position = { x1, y1 };
		mVertices[1].position = { x2, y2 };
		mVertices[2].position = { x3, y3 };
		mVertices[3].position = { x4, y4 };

		mVertices[0].color = mVertices[1].color = mVertices[2].color = mVertices[3].color = color;
	}


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		target.draw(mVertices, states);
	}


private:
	sf::VertexArray mVertices;
};

struct Point
{
	struct Screen
	{
		float x{};
		float y{};
		float w{};
		float scale{};

	} screen{};

	sf::Vector3f world{};
	sf::Vector3f camera{};

	void project(float cameraX, float cameraY, float cameraZ, float cameraDepth, float width, float height, float roadWidth)
	{
		camera.x = world.x - cameraX;
		camera.y = world.y - cameraY;
		camera.z = world.z - cameraZ;

		screen.scale = cameraDepth / camera.z;
		screen.x = width / 2u + screen.scale * camera.x  * width / 2u;
		screen.y = height / 2u - screen.scale * camera.y  * height / 2u;
		screen.w = screen.scale * roadWidth * width / 2u;
	}
};

class Colors
{
public:
	enum Type
	{
		Light,
		Dark,
		Start,
		Finish,
		TypeCount
	};


private:
	struct ColorsData
	{
		sf::Color road{};
		sf::Color grass{};
		sf::Color rumble{};
		sf::Color lanes{};
	};

	using ColorsContainer = std::vector<ColorsData>;


public:
	Colors(Type type = Colors::Light)
		: mType(type)
		, mData(Colors::TypeCount)
	{
		mData[Colors::Light].road = { 100, 100, 100 };
		mData[Colors::Light].grass = { 16, 170, 16 };
		mData[Colors::Light].rumble = { 85, 85 , 85 };
		mData[Colors::Light].lanes = { 204, 204, 204 };

		mData[Colors::Dark].road = { 100, 100, 100 };
		mData[Colors::Dark].grass = { 0, 154, 0 };
		mData[Colors::Dark].rumble = { 187,187, 187 };
		mData[Colors::Dark].lanes = { 100, 100, 100 };

		mData[Colors::Start].road = sf::Color::White;
		mData[Colors::Start].grass = { 16, 170, 16 };
		mData[Colors::Start].rumble = sf::Color::White;
		mData[Colors::Start].lanes = sf::Color::White;

		mData[Colors::Finish].road = {};
		mData[Colors::Finish].grass = { 16, 170, 16 };
		mData[Colors::Finish].rumble = {};
		mData[Colors::Finish].lanes = {};
	}

	void setType(Type type) { mType = type; }

	sf::Color getRoad() const { return mData[mType].road; }
	sf::Color getGrass() const { return mData[mType].grass; }
	sf::Color getRumble() const { return mData[mType].rumble; }
	sf::Color getLane() const { return mData[mType].lanes; }


private:
	Type mType;
	ColorsContainer mData;
};

struct SpritesData
{
	sf::IntRect PalmTree{ 5,  5,  215, 540 };
	sf::IntRect BillBoard08{ 230, 5, 385, 265 };
	sf::IntRect Tree1{ 625, 5, 360, 360 };
	sf::IntRect DeadTree1{ 5, 555, 135, 332 };
	sf::IntRect BillBoard09{ 150, 555, 328, 282 };
	sf::IntRect Boulder3{ 230,  280,  320,  220 };
	sf::IntRect Column{ 995, 5, 200, 315 };
	sf::IntRect BillBoard01{ 625, 375, 300,  170 };
	sf::IntRect BillBoard06{ 488, 555, 298, 190 };
	sf::IntRect BillBoard05{ 5, 897, 298, 190 };
	sf::IntRect BillBoard07{ 313, 897, 298,  190 };
	sf::IntRect Boulder2{ 621,  897,  298,  140 };
	sf::IntRect Tree2{ 1205,  5,  282,  295 };
	sf::IntRect BillBoard04{ 1205, 310,  268,  170 };
	sf::IntRect DeadTree2{ 1205,  490,  150,  260 };
	sf::IntRect Boulder1{ 1205,  760, 168, 248 };
	sf::IntRect Bush1{ 5,  1097,  240, 155 };
	sf::IntRect Cactus{ 929,  897, 235, 118 };
	sf::IntRect Bush2{ 255, 1097, 232, 152 };
	sf::IntRect BillBoard03{ 5, 1262, 230, 220 };
	sf::IntRect BillBoard02{ 245, 1262,215, 220 };
	sf::IntRect Stump{ 995, 330, 195,  140 };
	sf::IntRect Semi{ 1365, 490, 122, 144 };
	sf::IntRect Truck{ 1365,  644,  100, 78 };
	sf::IntRect Car03{ 1383, 760, 88, 55 };
	sf::IntRect Car02{ 1383,  825, 80,  59 };
	sf::IntRect Car04{ 1383,  894,  80,  57 };
	sf::IntRect Car01{ 1205,  1018, 80, 56 };
	sf::IntRect PlayerUpHillLeft{ 1383,  961,  80,  45 };
	sf::IntRect PlayerUpHillStraight{ 1295,  1018,  80, 45 };
	sf::IntRect PlayerUpHillRight{ 1385, 1018, 80,  45 };
	sf::IntRect PlayerLeft{ 995,  480,  80, 41 };
	sf::IntRect PlayerStraight{ 1085, 480, 80,  41 };
	sf::IntRect PlayerRight{ 995, 531, 80, 41 };

	float Scale = 0.3f * (1 / static_cast<float>(PlayerStraight.width));

	std::vector<sf::IntRect> Plants
	{
		Tree1,
		Tree2,
		DeadTree1,
		DeadTree2,
		PalmTree,
		Bush1,
		Bush2,
		Cactus,
		Stump,
		Boulder1,
		Boulder2,
		Boulder3
	};

	std::vector<sf::IntRect> BillBoads
	{
		BillBoard01,
		BillBoard02,
		BillBoard03,
		BillBoard04,
		BillBoard05,
		BillBoard06,
		BillBoard07,
		BillBoard08,
		BillBoard09
	};

	std::vector<sf::IntRect> Cars
	{
		Car01,
		Car02,
		Car03,
		Car04,
		Semi,
		Truck
	};

}spritesData;


class Sprites final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Sprites>;


public:
	Sprites(const sf::Texture& textures, sf::IntRect rect, float offset)
		: mSprite(textures, rect)
		, mIsDrawing(false)
		, mOffset(offset)
	{
	}

	float getOffset() const { return mOffset; }

	sf::FloatRect getBoundingRect() const
	{
		return getTransform().transformRect(mSprite.getGlobalBounds());
	}

	void update(float width, float roadWidth, float scaleXY, float destX, float destY, int offsetX, int offsetY, float clip)
	{
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


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		if (!mIsDrawing) return;

		states.transform *= getTransform();
		target.draw(mSprite, states);

		mIsDrawing = false;
	}


private:
	sf::Sprite mSprite;
	mutable bool mIsDrawing;
	float  mOffset;
};

class Segment final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Segment>;


private:
	using SpritesContainer = std::vector<Sprites::Ptr>;


public:
	Segment(const sf::Texture& textures) : mTextures(textures) {}

	void setCurve(float i) { mCurve = i; }
	float getCurve() const { return mCurve; }

	Point&  point1() { return mPoint1; }
	Point&  point2() { return mPoint2; }
	const Point&  point1() const { return mPoint1; }
	const Point&  point2() const { return mPoint2; }

	void setSegmentColors(Colors::Type color) { mColors.setType(color); }

	void setIndex(std::size_t index) { mIndex = index; }
	std::size_t getIndex() const { return mIndex; }

	void setClip(float clip) { mClip = clip; }
	const float getClip() const { return mClip; }

	const SpritesContainer& getSprites() const { return mSprites; };

	void addSprite(sf::IntRect spriteRect, float offset)
	{
		auto sprite = std::make_unique<Sprites>(mTextures, spriteRect, offset);
		mSprites.push_back(std::move(sprite));
	}

	void setGrounds(float width, float fog)
	{
		auto lanes = 3u;

		// Landscape
		mLandscape.setSize({ width, mPoint1.screen.y - mPoint2.screen.y });
		mLandscape.setPosition(0, mPoint2.screen.y);
		mLandscape.setFillColor(mColors.getGrass());

		// Rumble sides
		auto rumbleWidth1 = rumbleWidth(mPoint1.screen.w, lanes);
		auto rumbleWidth2 = rumbleWidth(mPoint2.screen.w, lanes);

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
		auto laneMarkerWidth1 = laneMarkerWidth(mPoint1.screen.w, lanes);
		auto laneMarkerWidth2 = laneMarkerWidth(mPoint2.screen.w, lanes);
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


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
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


private:
	Point mPoint1{};
	Point mPoint2{};
	Polygon mRumbleSide1{};
	Polygon mRumbleSide2{};
	Polygon mLanes1{};
	Polygon mLanes2{};
	Polygon mMainRoad{};
	sf::RectangleShape mLandscape{};
	sf::RectangleShape mFog{};
	const sf::Texture& mTextures;
	SpritesContainer mSprites{};
	std::size_t mIndex{};
	Colors mColors{};
	float mCurve{};
	float mClip{};
};

class Background final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	enum Type
	{
		Sky,
		Hills,
		Trees
	};

	using Ptr = std::unique_ptr<Background>;


private:
	using SkyContainer = std::array<sf::Sprite, 2>;


public:
	Background(Type type, const sf::Texture& texture)
		: mType(type)
		, mSprite()
		, mSky()
		, mSkyTexureWidth(static_cast<float>(texture.getSize().x))
	{
		mSky[0].setTexture(texture);
		mSky[0].setPosition(-mSkyTexureWidth, 0.f);
		mSky[1].setTexture(texture);
	}

	explicit Background(Type type, const sf::Texture& texture, sf::IntRect rect)
		: mType(type)
		, mSprite(texture, rect)
		, mSky()
		, mSkyTexureWidth()
	{
	}

	void update(float dt, float curve, float moving)
	{
		if (mType == Background::Sky)
		{
			auto skySpeed = 10.f;

			move(skySpeed * dt, 0.f);

			if (getPosition().x > mSkyTexureWidth)
				move(-mSkyTexureWidth, 0.f);

			return;
		}

		if (moving == 0) return;

		if (mType == Background::Hills)
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


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		if (mType == Background::Sky)
		{
			for (const auto& sky : mSky)
				target.draw(sky, states);
		}
		else
		{
			target.draw(mSprite, states);
		}
	}


private:
	Type mType;
	sf::Sprite mSprite;
	SkyContainer mSky;
	float mSkyTexureWidth;
};

class Player final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Player>;


public:
	Player(const sf::Texture& texture, sf::IntRect rect)
		: mSprite(texture, rect)
	{
	}

	sf::FloatRect getBoundingRect() const
	{
		return getTransform().transformRect(mSprite.getGlobalBounds());
	}

	void update(float width, float roadWidth, float scaleXY, float destX, float destY, int steer, float updown)
	{
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


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		target.draw(mSprite, states);
	}


private:
	sf::Sprite mSprite;
};

class Road final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	using Ptr = std::unique_ptr<Road>;


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

	struct 	Hill
	{
		const float none = 0.f;
		const float low = 20.f;
		const float medium = 40.f;
		const float high = 60.f;
	} hill;

	using SegmentContainer = std::vector<Segment::Ptr>;


public:
	Road(const sf::Texture& texture)
		: mSegments()
		, mPlayer(nullptr)
		, mSegmentLength(200.f)
		, mPlayerX(0.f)
		, mCameraDepth(1 / std::tan((100.f / 2.f) * static_cast<float>(M_PI / 180.f)))
		, mCameraHeight(1000.f)
		, mPlayerZ((mCameraHeight * mCameraDepth))
		, mPosition(0.f)
		, mRumbleLength(3u)
		, mTrackLength(0.f)
		, mSpeed(0.f)
		, mCurve()
		, mSteer()
		, mTextures(texture)
		, mMaxSpeed(mSegmentLength / (1 / 60.f))
	{
		mPlayer = std::make_unique<Player>(mTextures, spritesData.PlayerStraight);

		// build road
		addStraight(length.shorty);
		addLowRollingHills();
		addSCurves();
		addCurve(length.medium, curve.medium, hill.low);
		addBumps();
		addLowRollingHills();
		addCurve(length.longy * 2, curve.medium, hill.medium);
		addStraight();
		addHill(length.medium, hill.high);
		addSCurves();
		addCurve(length.longy, -curve.medium, hill.none);
		addHill(length.longy, hill.high);
		addCurve(length.longy, curve.medium, -hill.low);
		addBumps();
		addHill(length.longy, -hill.medium);
		addStraight();
		addSCurves();
		addDownhillToEnd();

		// add side sprites
		addSprite(20u, spritesData.BillBoard07, -1.f);
		addSprite(40u, spritesData.BillBoard06, -1.f);
		addSprite(60u, spritesData.BillBoard08, -1.f);
		addSprite(80u, spritesData.BillBoard09, -1.f);
		addSprite(100u, spritesData.BillBoard01, -1.f);
		addSprite(120u, spritesData.BillBoard02, -1.f);
		addSprite(140u, spritesData.BillBoard03, -1.f);
		addSprite(160u, spritesData.BillBoard04, -1.f);
		addSprite(180u, spritesData.BillBoard05, -1.f);
		addSprite(240u, spritesData.BillBoard07, -1.2f);
		addSprite(240u, spritesData.BillBoard06, 1.2f);

		addSprite(mSegments.size() - 25, spritesData.BillBoard07, -1.2f);
		addSprite(mSegments.size() - 25, spritesData.BillBoard06, 1.2f);

		for (auto n = 10u; n < 240; n += 4 + n / 100u)
		{
			addSprite(n, spritesData.PalmTree, random(0.6f, 0.9f));
			addSprite(n, spritesData.PalmTree, random(1.5f, 5.5f));
		}

		for (auto n = 250u; n < 1000; n += 5)
		{
			addSprite(n, spritesData.Column, 1.1f);
			addSprite(n + random(0, 5), spritesData.Tree1, -random(1.f, 5.5f));
			addSprite(n + random(0, 5), spritesData.Tree2, -random(1.f, 5.5f));
		}

		std::vector<float> vec{	1.f, -1.f };

		for (auto n = 200u; n < mSegments.size(); n += 3)
		{
			addSprite(n, randomChoice(spritesData.Plants), randomChoice(vec) * random(1.f, 5.5f));
		}

		for (auto n = 1000u; n < (mSegments.size() - 50); n += 100)
		{
			auto side = randomChoice(vec);
			addSprite(n + random(0, 50), randomChoice(spritesData.BillBoads), -side);
			for (auto i = 0u; i < 20; i++)
			{
				addSprite(n + random(0, 50), randomChoice(spritesData.Plants), side * random(1.f, 5.5f));
			}
		}

		// setup start and finish of road
		mSegments[mSegments[static_cast<std::size_t>(std::floor(mPlayerZ / mSegmentLength)) % mSegments.size()]->getIndex() + 2]->setSegmentColors(Colors::Start);
		mSegments[mSegments[static_cast<std::size_t>(std::floor(mPlayerZ / mSegmentLength)) % mSegments.size()]->getIndex() + 3]->setSegmentColors(Colors::Start);

		for (auto n = 0u; n < mRumbleLength; n++)
		{
			mSegments[mSegments.size() - 1 - n]->setSegmentColors(Colors::Finish);
		}

		mTrackLength = mSegments.size() * mSegmentLength;
	}

	void update(float dt)
	{
		auto accel = mMaxSpeed / 5.f;
		auto breaking = -mMaxSpeed;
		auto decel = -accel;
		auto offRoadDecel = -mMaxSpeed / 2.f;
		auto offRoadLimit = mMaxSpeed / 4.f;
		auto centrifugal = 0.3f;
		bool keyLeft = false;
		bool keyRight = false;

		const auto& baseSegment = *mSegments[static_cast<std::size_t>(std::floor(mPosition / mSegmentLength)) % mSegments.size()];
		const auto& playerSegment = *mSegments[static_cast<std::size_t>(std::floor((mPosition + mPlayerZ) / mSegmentLength)) % mSegments.size()];
		auto speedPercent = mSpeed / mMaxSpeed;
		auto dx = dt * speedPercent;


		mCurve = playerSegment.getCurve();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			keyLeft = true;
			mPlayerX -= dx;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			keyRight = true;
			mPlayerX += dx;
		}

		mPlayerX -= (dx * speedPercent * mCurve * centrifugal);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			mSpeed += accel * dt;
		else
			mSpeed += decel * dt;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			mSpeed += breaking * dt;

		if ((mPlayerX < -1.f) || (mPlayerX > 1.f))
		{
			if (mSpeed > offRoadLimit)
				mSpeed += offRoadDecel * dt;

			for (const auto& sprite : playerSegment.getSprites())
			{
				if (sprite->getBoundingRect().intersects(mPlayer->getBoundingRect()))
				{
					mSpeed = accel;
					mPosition = increase(playerSegment.point1().world.z, -mPlayerZ, mTrackLength);
					break;
				}
			}
		}

		mPlayerX = limit(mPlayerX, -2.f, 2.f);
		mSpeed = limit(mSpeed, 0, mMaxSpeed);

		mPosition = increase(mPosition, dt * mSpeed, mTrackLength);
		mSteer = (keyLeft ? -1 : keyRight ? 1 : 0);
	}

	float getCurve() const { return mCurve; }
	float getSpeed() const { return mSpeed; }


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		auto width = 640.f;
		auto height = 480.f;
		auto roadWidth = 2000.f;
		auto drawDistance = 500u;
		auto fogDensity = 5.f;

		const auto& baseSegment = *mSegments[static_cast<std::size_t>(std::floor(mPosition / mSegmentLength)) % mSegments.size()];
		auto basePercent = percentRemaining(mPosition, mSegmentLength);

		const auto& playerSegment = *mSegments[static_cast<std::size_t>(std::floor((mPosition + mPlayerZ) / mSegmentLength)) % mSegments.size()];
		auto playerPercent = percentRemaining(mPosition + mPlayerZ, mSegmentLength);
		auto playerY = interpolate(playerSegment.point1().world.y, playerSegment.point2().world.y, playerPercent);

		auto x = 0.f;
		auto dx = -(baseSegment.getCurve() * basePercent);
		auto maxy = height;

		for (auto n = 0u; n < drawDistance; ++n)
		{
			auto& segment = *mSegments[(baseSegment.getIndex() + n) % mSegments.size()];
			segment.setClip(maxy);

			bool looped = segment.getIndex() < baseSegment.getIndex();
			auto fog = exponentialFog(n / static_cast<float>(drawDistance), fogDensity);

			auto camX = mPlayerX * roadWidth;
			auto camY = playerY + mCameraHeight;
			auto camZ = mPosition - (looped ? mTrackLength : 0.f);

			auto& point1 = segment.point1();
			auto& point2 = segment.point2();

			point1.project(camX - x, camY, camZ, mCameraDepth, width, height, roadWidth);
			point2.project(camX - x - dx, camY, camZ, mCameraDepth, width, height, roadWidth);

			x += dx;
			dx += segment.getCurve();

			if ((point1.camera.z <= mCameraDepth) || (point2.screen.y >= maxy || point2.screen.y >= point1.screen.y))
				continue;

			segment.setGrounds(roadWidth, fog);

			target.draw(segment, states);

			maxy = point2.screen.y;
		}

		// render roadside sprites
		for (auto n = (drawDistance - 1); n > 0u; --n)
		{
			const auto& segment = *mSegments[(baseSegment.getIndex() + n) % mSegments.size()];

			for (const auto& sprite : segment.getSprites())
			{
				auto spriteScale = segment.point1().screen.scale;
				auto spriteX = segment.point1().screen.x + (spriteScale * sprite->getOffset() * roadWidth * width / 2);
				auto spriteY = segment.point1().screen.y;

				sprite->update(width, roadWidth,
					spriteScale,
					spriteX,
					spriteY,
					(sprite->getOffset() < 0 ? -1 : 0), -1,
					segment.getClip());

				target.draw(*sprite, states);
			}

			// player
			if (segment.getIndex() == playerSegment.getIndex())
			{
				mPlayer->update(width, roadWidth,
					mCameraDepth / mPlayerZ,
					width / 2,
					(height / 2) - (mCameraDepth / mPlayerZ * interpolate(playerSegment.point1().camera.y, playerSegment.point2().camera.y, playerPercent) * height / 2),
					mSteer,
					playerSegment.point2().world.y - playerSegment.point1().world.y);

				target.draw(*mPlayer, states);
			}
		}
	}

	void addSegment(float curve, float y)
	{
		auto n = mSegments.size();

		auto segment = std::make_unique<Segment>(mTextures);

		segment->setIndex(n);
		segment->setCurve(curve);

		segment->point1().world.y = lastY();
		segment->point2().world.y = y;

		segment->point1().world.z = n * mSegmentLength;
		segment->point2().world.z = (n + 1) * mSegmentLength;

		if (static_cast<std::size_t>(std::floor(n / mRumbleLength)) % 2)
			segment->setSegmentColors(Colors::Light);
		else
			segment->setSegmentColors(Colors::Dark);

		mSegments.push_back(std::move(segment));
	}

	void addSprite(std::size_t n, sf::IntRect sprite, float offset)
	{
		mSegments[n]->addSprite(sprite, offset);
	}

	void addRoad(float enter, float hold, float leave, float curve, float y)
	{
		auto startY = lastY();
		auto endY = startY + (y * mSegmentLength);
		auto total = enter + hold + leave;

		for (auto n = 0.f; n < enter; ++n)
			addSegment(easeIn(0, curve, n / enter), easeInOut(startY, endY, n / total));

		for (auto n = 0.f; n < hold; ++n)
			addSegment(curve, easeInOut(startY, endY, (enter + n) / total));

		for (auto n = 0.f; n < leave; ++n)
			addSegment(easeInOut(curve, 0, n / leave), easeInOut(startY, endY, (enter + hold + n) / total));
	}

	void  addStraight(float n = 0)
	{
		auto num = (n == 0) ? length.medium : n;
		addRoad(num, num, num, 0, 0);
	}

	void addHill(float n = 0, float h = 0)
	{
		auto num = (n == 0) ? length.medium : n;
		auto height = (h == 0) ? hill.medium : h;

		addRoad(num, num, num, 0, height);
	}

	void addCurve(float n = 0, float c = 0, float h = 0)
	{
		auto num = (n == 0) ? length.medium : n;
		auto curves = (c == 0) ? curve.medium : c;
		auto height = (h == 0) ? hill.none : h;

		addRoad(num, num, num, curves, height);
	}

	void addDownhillToEnd(float n = 0)
	{
		auto num = (n == 0) ? 200 : n;
		addRoad(num, num, num, -curve.easy, -lastY() / mSegmentLength);
	}

	void addSCurves()
	{
		addRoad(length.medium, length.medium, length.medium, -curve.easy, hill.none);
		addRoad(length.medium, length.medium, length.medium, curve.medium, hill.medium);
		addRoad(length.medium, length.medium, length.medium, curve.easy, -hill.low);
		addRoad(length.medium, length.medium, length.medium, -curve.easy, hill.medium);
		addRoad(length.medium, length.medium, length.medium, -curve.medium, -hill.medium);
	}

	void addLowRollingHills(float n = 0, float h = 0)
	{
		auto num = (n == 0) ? length.shorty : n;
		auto height = (h == 0) ? hill.low : h;

		addRoad(num, num, num, 0, height / 2);
		addRoad(num, num, num, 0, -height);
		addRoad(num, num, num, 0, height);
		addRoad(num, num, num, 0, 0);
		addRoad(num, num, num, 0, height / 2);
		addRoad(num, num, num, 0, 0);
	}

	void addBumps()
	{
		addRoad(10, 10, 10, 0, 5);
		addRoad(10, 10, 10, 0, -2);
		addRoad(10, 10, 10, 0, -5);
		addRoad(10, 10, 10, 0, 8);
		addRoad(10, 10, 10, 0, 5);
		addRoad(10, 10, 10, 0, -7);
		addRoad(10, 10, 10, 0, 5);
		addRoad(10, 10, 10, 0, -2);
	}

	float lastY()
	{
		return (mSegments.size() == 0) ? 0 : mSegments[mSegments.size() - 1]->point2().world.y;
	}


private:
	const sf::Texture& mTextures;
	SegmentContainer mSegments;
	std::size_t mRumbleLength;
	Player::Ptr mPlayer;
	float mSegmentLength;
	float mPlayerX;
	float mCameraDepth;
	float mCameraHeight;
	float mPlayerZ;
	float mPosition;
	float mMaxSpeed;
	float mTrackLength;
	float mSpeed;
	float mCurve;
	int mSteer;

};

class World final : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
	using BackgroundContainer = std::vector<Background::Ptr>;


public:
	World(std::size_t ScreenHeight)
		: mRoad(nullptr)
		, mBackground()
		, mTextures()
		, mHillTexture()
		, mSkyTexture()
		, mTreesTexture()
	{
		// load resources
		if (!mTextures.loadFromFile("Media/sprites.png")
			|| !mHillTexture.loadFromFile("Media/hills.png")
			|| !mSkyTexture.loadFromFile("Media/sky.png")
			|| !mTreesTexture.loadFromFile("Media/trees.png"))
		{
			throw std::runtime_error("can't load resources");
		}

		// setup sprites
		mRoad = std::make_unique<Road>(mTextures);

		auto sky = std::make_unique<Background>(Background::Sky, mSkyTexture);
		mBackground.push_back(std::move(sky));

		sf::IntRect hillRect(0, 0, 2000, ScreenHeight);
		mHillTexture.setRepeated(true);
		auto hill = std::make_unique<Background>(Background::Hills, mHillTexture, hillRect);
		hill->setPosition(-hillRect.width / 2.f, 0.f);
		mBackground.push_back(std::move(hill));

		sf::IntRect treeRect(0, 0, 3000, ScreenHeight);
		mTreesTexture.setRepeated(true);
		auto tree = std::make_unique<Background>(Background::Trees, mTreesTexture, treeRect);
		tree->setPosition(-hillRect.width / 2.f, 0.f);
		mBackground.push_back(std::move(tree));
	}

	void update(float dt)
	{
		mRoad->update(dt);

		for (const auto& object : mBackground)
			object->update(dt, mRoad->getCurve(), mRoad->getSpeed());
	}


private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		for (const auto& object : mBackground)
			target.draw(*object, states);

		target.draw(*mRoad, states);
	}


private:
	Road::Ptr mRoad;
	sf::Texture mTextures;

	BackgroundContainer mBackground;
	sf::Texture mSkyTexture;
	sf::Texture mHillTexture;
	sf::Texture mTreesTexture;
};

class Game
{
public:
	Game()
		: mWindow(sf::VideoMode(640, 480), "test")
		, mWorld(mWindow.getSize().y)
	{
	}

	void run()
	{
		sf::Clock clock;
		auto timeSinceLastUpdate = sf::Time::Zero;

		while (mWindow.isOpen())
		{
			auto elapsedTime = clock.restart();
			timeSinceLastUpdate += elapsedTime;

			while (timeSinceLastUpdate > TimePerFrame)
			{
				timeSinceLastUpdate -= TimePerFrame;

				processEvents();
				update(TimePerFrame);
			}

			render();
		}
	}


private:
	void processEvents()
	{
		sf::Event event;

		while (mWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mWindow.close();
		}
	}

	void update(sf::Time TimePerFrame)
	{
		mWorld.update(TimePerFrame.asSeconds());
	}

	void render()
	{
		mWindow.clear();
		mWindow.draw(mWorld);
		mWindow.display();
	}


private:
	const static sf::Time TimePerFrame;

	sf::RenderWindow mWindow;
	World mWorld;
};

const sf::Time Game::TimePerFrame = sf::seconds(1 / 60.f);

int main()
{
	try
	{
		Game game;
		game.run();
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
}