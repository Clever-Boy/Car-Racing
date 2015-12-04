#include "Colors.hpp"


Colors::Colors(Type type)
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

void Colors::setType(Type type)
{ 
	mType = type; 
}

sf::Color Colors::getRoad() const
{ 
	return mData[mType].road;
}

sf::Color Colors::getGrass() const
{ 
	return mData[mType].grass; 
}

sf::Color Colors::getRumble() const
{
	return mData[mType].rumble; 
}

sf::Color Colors::getLane() const 
{ 
	return mData[mType].lanes;
}