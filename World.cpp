#include "World.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

using namespace utility;


World::World(sf::RenderWindow& window)
	: mWindow(window)
	, mTextures()
	, mSegments()
	, mBackground()
	, mCars()
	, mPlayer(nullptr)
	, mSegmentLength(200.f)
	, mCameraDepth(1 / std::tan((100.f / 2.f) * static_cast<float>(M_PI / 180.f)))
	, mCameraHeight(1000.f)
	, mPosition()
	, mRumbleLength(3u)
	, mDrawDistance(500u)
	, mTrackLength()
	, mMaxSpeed(mSegmentLength / (1 / 60.f))
	, mSteer()

{
	loadTextures();
	buildScene();
}

void World::update(float dt)
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
	const auto& playerSegment = *mSegments[static_cast<std::size_t>(std::floor((mPosition + mPlayer->getZValue()) / mSegmentLength)) % mSegments.size()];
	auto speedPercent = mPlayer->getSpeed() / mMaxSpeed;
	auto dx = dt * speedPercent;

	// update cars
	for (const auto& car : mCars)
	{
		auto& oldSegment = *mSegments[static_cast<std::size_t>(std::floor(car->getZValue() / mSegmentLength)) % mSegments.size()];
		car->setOffset(car->getOffset() + updateCarOffset(car, oldSegment, playerSegment));
		car->setZValue(increase(car->getZValue(), dt * car->getSpeed(), mTrackLength));
		car->setPercent(percentRemaining(car->getZValue(), mSegmentLength));
		auto& newSegment = *mSegments[static_cast<std::size_t>(std::floor(car->getZValue() / mSegmentLength)) % mSegments.size()];

		if (oldSegment.getIndex() == newSegment.getIndex())
			continue;

		oldSegment.removeCar();
		newSegment.addCar(car);
	}

	// update player
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		keyLeft = true;
		mPlayer->moveLeft(dx);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		keyRight = true;
		mPlayer->moveRight(dx);
	}

	mPlayer->adaptMoving((dx * speedPercent * playerSegment.getCurve() * centrifugal));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		mPlayer->accelerate(accel * dt);
	else
		mPlayer->accelerate(decel * dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		mPlayer->accelerate(breaking * dt);

	if ((mPlayer->getOffset() < -1.f) || (mPlayer->getOffset() > 1.f))
	{
		if (mPlayer->getSpeed() > offRoadLimit)
			mPlayer->accelerate(offRoadDecel * dt);

		// check for collision with side road objects
		const auto& spriteVector = playerSegment.getSprites();
		for (const auto& sprite : spriteVector)
		{
			if (!sprite->getBoundingRect().intersects(mPlayer->getBoundingRect()))
				continue;

			mPlayer->setSpeed(accel);
			mPosition = increase(playerSegment.point1().world.z, -mPlayer->getZValue(), mTrackLength);
			break;
		}
	}

	// check for collision with cars
	const auto& carsVector = playerSegment.getCars();
	for (const auto& car : carsVector)
	{
		if (mPlayer->getSpeed() <= car->getSpeed())
			continue;

		if (!car->getBoundingRect().intersects(mPlayer->getBoundingRect()))
			continue;

		mPlayer->setSpeed(car->getSpeed() * (car->getSpeed() / mPlayer->getSpeed()));
		mPosition = increase(car->getZValue(), -mPlayer->getZValue(), mTrackLength);
		break;
	}

	mPlayer->setOffset(limit(mPlayer->getOffset(), -2.f, 2.f));
	mPlayer->setSpeed(limit(mPlayer->getSpeed(), 0, mMaxSpeed));

	mPosition = increase(mPosition, dt * mPlayer->getSpeed(), mTrackLength);
	mSteer = (keyLeft ? -1 : keyRight ? 1 : 0);

	// update backgrounds
	for (const auto& object : mBackground)
		object->update(dt, playerSegment.getCurve(), mPlayer->getSpeed());
}

void World::draw()
{
	auto width = 640.f;
	auto height = 480.f;
	auto roadWidth = 2000.f;
	auto fogDensity = 5.f;

	const auto& baseSegment = *mSegments[static_cast<std::size_t>(std::floor(mPosition / mSegmentLength)) % mSegments.size()];
	auto basePercent = percentRemaining(mPosition, mSegmentLength);

	const auto& playerSegment = *mSegments[static_cast<std::size_t>(std::floor((mPosition + mPlayer->getZValue()) / mSegmentLength)) % mSegments.size()];
	auto playerPercent = percentRemaining(mPosition + mPlayer->getZValue(), mSegmentLength);
	auto playerY = interpolate(playerSegment.point1().world.y, playerSegment.point2().world.y, playerPercent);

	auto x = 0.f;
	auto dx = -(baseSegment.getCurve() * basePercent);
	auto maxy = height;

	for (const auto& object : mBackground)
		mWindow.draw(*object);

	// render segments
	for (auto n = 0u; n < mDrawDistance; ++n)
	{
		auto& segment = *mSegments[(baseSegment.getIndex() + n) % mSegments.size()];
		segment.setClip(maxy);

		bool looped = segment.getIndex() < baseSegment.getIndex();
		auto fog = exponentialFog(n / static_cast<float>(mDrawDistance), fogDensity);

		auto camX = mPlayer->getOffset() * roadWidth;
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

		mWindow.draw(segment);

		maxy = point2.screen.y;
	}

	for (auto n = (mDrawDistance - 1); n > 0; --n)
	{
		const auto& segment = *mSegments[(baseSegment.getIndex() + n) % mSegments.size()];

		// render side road sprites
		const auto& spriteVector = segment.getSprites();
		for (const auto& sprite : spriteVector)
		{
			auto spriteScale = segment.point1().screen.scale;
			auto spriteX = segment.point1().screen.x + spriteScale * sprite->getOffset() * roadWidth * width / 2;
			auto spriteY = segment.point1().screen.y;
			auto offsetX = sprite->getOffset() < 0 ? -1.f : 0.f;
			auto offsetY = -1.f;
			sprite->update(width, roadWidth, spriteScale, spriteX, spriteY, offsetX, offsetY, segment.getClip());

			mWindow.draw(*sprite);
		}

		// cars
		const auto& carsVector = segment.getCars();
		for (const auto& car : carsVector)
		{
			auto spriteScale = interpolate(segment.point1().screen.scale, segment.point2().screen.scale, car->getPercent());
			auto interpolateValue = interpolate(segment.point1().screen.x, segment.point2().screen.x, car->getPercent());
			auto spriteX = interpolateValue + spriteScale * car->getOffset() * roadWidth * width / 2;
			auto spriteY = interpolate(segment.point1().screen.y, segment.point2().screen.y, car->getPercent());
			car->update(width, roadWidth, spriteScale, spriteX, spriteY, -0.5f, -1.f, segment.getClip());

			mWindow.draw(*car);
		}

		// player
		if (segment.getIndex() != playerSegment.getIndex())
			continue;

		auto spriteScale = mCameraDepth / mPlayer->getZValue();
		auto interpolateValue = interpolate(playerSegment.point1().camera.y, playerSegment.point2().camera.y, playerPercent);
		auto correctionValue = mCameraDepth / mPlayer->getZValue() * interpolateValue * height / 2;
		auto clip = playerSegment.point2().world.y - playerSegment.point1().world.y;
		auto spriteX = width / 2;
		auto spriteY = height / 2 - correctionValue;
		mPlayer->update(width, roadWidth, spriteScale, spriteX, spriteY, mSteer, clip);

		mWindow.draw(*mPlayer);
	}
}

void World::loadTextures()
{
	mTextures.load(Textures::Sprites, "Media/sprites.png");
	mTextures.load(Textures::Sky, "Media/sky.png");
	mTextures.load(Textures::Hills, "Media/hills.png");
	mTextures.load(Textures::Trees, "Media/trees.png");
}

void World::buildScene()
{
	// setup sprites
	auto& skyTexture = mTextures.get(Textures::Sky);
	auto sky = std::make_unique<Backgrounds>(Backgrounds::Sky, skyTexture);
	mBackground.push_back(std::move(sky));

	auto& hillsTexture = mTextures.get(Textures::Hills);
	hillsTexture.setRepeated(true);
	sf::IntRect hillRect(0, 0, 4000, mWindow.getSize().y);
	auto hills = std::make_unique<Backgrounds>(Backgrounds::Hills, hillsTexture, hillRect);
	hills->setPosition(-hillRect.width / 2.f, 0.f);
	mBackground.push_back(std::move(hills));

	auto& treesTexture = mTextures.get(Textures::Trees);
	treesTexture.setRepeated(true);
	sf::IntRect treeRect(0, 0, 6000, mWindow.getSize().y);
	auto tree = std::make_unique<Backgrounds>(Backgrounds::Trees, treesTexture, treeRect);
	tree->setPosition(-treeRect.width / 2.f, 0.f);
	mBackground.push_back(std::move(tree));

	// add player
	SpritesData	spritesData;
	mPlayer = std::make_unique<Player>(mTextures, spritesData.PlayerStraight, mCameraHeight * mCameraDepth);

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
		addSprite(n + random(0, 5), spritesData.Tree1, -random(1.1f, 5.5f));
		addSprite(n + random(0, 5), spritesData.Tree2, -random(1.1f, 5.5f));
	}

	std::vector<float> vec{ 1.f, -1.f };

	for (auto n = 200u; n < mSegments.size(); n += 3)
	{
		addSprite(n, randomChoice(spritesData.Plants), randomChoice(vec) * random(1.1f, 5.5f));
	}

	for (auto n = 1000u; n < (mSegments.size() - 50); n += 100)
	{
		auto side = randomChoice(vec);
		addSprite(n + random(0, 50), randomChoice(spritesData.BillBoads), -side);
		for (auto i = 0u; i < 20; i++)
		{
			addSprite(n + random(0, 50), randomChoice(spritesData.Plants), side * random(1.1f, 5.5f));
		}
	}

	// add Cars
	std::vector<float> vecCars{ 0.9f, 0.f, -0.6f };
	auto totalCars = 100u;
	for (auto n = 0u; n < totalCars; n++)
	{
		auto offset = randomChoice(vecCars);
		auto z = random(0u, mSegments.size()) * mSegmentLength;
		auto sprite = spritesData.Car01; //randomChoice(spritesData.Cars);
		auto speed = mMaxSpeed / 4.f + random(0.f, 0.7f) * mMaxSpeed / (sprite == spritesData.Semi ? 4.f : 2.f);
		auto car = std::make_shared<Cars>(mTextures, sprite, offset, z, speed);
		auto& segment = *mSegments[static_cast<std::size_t>(std::floor(z / mSegmentLength)) % mSegments.size()];
		mCars.push_back(car);
		segment.addCar(car);
	}

	// setup start and finish of road marks
	mSegments[mSegments[static_cast<std::size_t>(std::floor(mPlayer->getZValue() / mSegmentLength)) % mSegments.size()]->getIndex() + 2]->setSegmentColors(Colors::Start);
	mSegments[mSegments[static_cast<std::size_t>(std::floor(mPlayer->getZValue() / mSegmentLength)) % mSegments.size()]->getIndex() + 3]->setSegmentColors(Colors::Start);

	for (auto n = 0u; n < mRumbleLength; n++)
	{
		mSegments[mSegments.size() - 1 - n]->setSegmentColors(Colors::Finish);
	}

	mTrackLength = mSegments.size() * mSegmentLength;
}

void World::addSegment(float curve, float y)
{
	auto n = mSegments.size();

	auto segment = std::make_unique<Segment>();

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

void World::addSprite(std::size_t n, const sf::IntRect& spriteRect, float offset)
{
	auto sprite = std::make_unique<Sprites>(mTextures, spriteRect, offset);
	mSegments[n]->addSprite(std::move(sprite));
}

void World::addRoad(float enter, float hold, float leave, float curve, float y)
{
	auto startY = lastY();
	auto endY = startY + (y * mSegmentLength);
	auto total = enter + hold + leave;

	for (auto n = 0.f; n < enter; n++)
		addSegment(easeIn(0, curve, n / enter), easeInOut(startY, endY, n / total));

	for (auto n = 0.f; n < hold; n++)
		addSegment(curve, easeInOut(startY, endY, (enter + n) / total));

	for (auto n = 0.f; n < leave; n++)
		addSegment(easeInOut(curve, 0, n / leave), easeInOut(startY, endY, (enter + hold + n) / total));
}

void World::addStraight(float n)
{
	auto num = (n == 0) ? length.medium : n;
	addRoad(num, num, num, 0, 0);
}

void World::addHill(float n, float h)
{
	auto num = (n == 0) ? length.medium : n;
	auto height = (h == 0) ? hill.medium : h;

	addRoad(num, num, num, 0, height);
}

void World::addCurve(float n, float c, float h)
{
	auto num = (n == 0) ? length.medium : n;
	auto curves = (c == 0) ? curve.medium : c;
	auto height = (h == 0) ? hill.none : h;

	addRoad(num, num, num, curves, height);
}

void World::addDownhillToEnd(float n)
{
	auto num = (n == 0) ? 200.f : n;
	addRoad(num, num, num, -curve.easy, -lastY() / mSegmentLength);
}

void World::addSCurves()
{
	addRoad(length.medium, length.medium, length.medium, -curve.easy, hill.none);
	addRoad(length.medium, length.medium, length.medium, curve.medium, hill.medium);
	addRoad(length.medium, length.medium, length.medium, curve.easy, -hill.low);
	addRoad(length.medium, length.medium, length.medium, -curve.easy, hill.medium);
	addRoad(length.medium, length.medium, length.medium, -curve.medium, -hill.medium);
}

void World::addLowRollingHills(float n, float h)
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

void World::addBumps()
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

float World::lastY()
{
	return (mSegments.size() == 0) ? 0 : mSegments[mSegments.size() - 1]->point2().world.y;
}

float World::updateCarOffset(Cars::Ptr car, const Segment& carSegment, const Segment& playerSegment)
{
	// optimization, dont bother steering around other cars when 'out of sight' of the player
	if (std::abs(static_cast<float>(carSegment.getIndex()) - static_cast<float>(playerSegment.getIndex())) > mDrawDistance)
		return 0.f;

	auto dir = 0;

	for (auto i = 1u; i < 20; ++i)
	{
		const auto& segment = *mSegments[(carSegment.getIndex() + i) % mSegments.size()];

		if (segment.getIndex() < playerSegment.getIndex()
			&& car->getSpeed() > mPlayer->getSpeed()
			&& car->getBoundingRect().intersects(mPlayer->getBoundingRect()))
		{
			if (mPlayer->getOffset() > 0.5f)
				dir = -1;
			else if (mPlayer->getOffset() < -0.5f)
				dir = 1;
			else
				dir = car->getOffset() > mPlayer->getOffset() ? 1 : -1;

			// the closer the cars (smaller i) and the greated the speed ratio, the larger the offset
			return dir * 1 / static_cast<float>(i) * (car->getSpeed() - mPlayer->getSpeed()) / mMaxSpeed;
		}

		const auto& carVector = segment.getCars();
		for (const auto& otherCar : carVector)
		{
			if (car->getSpeed() > otherCar->getSpeed() && car->getBoundingRect().intersects(otherCar->getBoundingRect()))
			{
				if (otherCar->getOffset() > 0.5f)
					dir = -1;
				else if (otherCar->getOffset() < -0.5f)
					dir = 1;
				else
					dir = car->getOffset() > otherCar->getOffset() ? 1 : -1;

				return dir * 1 / static_cast<float>(i) * (car->getSpeed() - otherCar->getSpeed()) / mMaxSpeed;
			}
		}
	}

	// if no cars ahead, but I have somehow ended up off road, then steer back on
	if (car->getOffset() < -0.9f)
		return 0.1f;
	else if (car->getOffset() > 0.9f)
		return -0.1f;
	else
		return 0.f;
}