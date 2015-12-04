#pragma once


#include <SFML/Graphics/Color.hpp>

#include <vector>


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
	explicit				Colors(Type type = Type::Light);

	void					setType(Type type);

	sf::Color				getRoad() const;
	sf::Color				getGrass() const;
	sf::Color				getRumble() const;
	sf::Color				getLane() const;


private:
	Type					mType;
	ColorsContainer			mData;
};