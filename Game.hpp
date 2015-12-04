#pragma once


#include "World.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


class Game : private sf::NonCopyable
{
public:
							Game();

	void					run();


private:
	void					processEvents();
	void					update(sf::Time TimePerFrame);
	void					render();


private:
	const static sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	World					mWorld;
};