#include "Game.hpp"

#include <SFML/Window/Event.hpp>


const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);


Game::Game()
	: mWindow(sf::VideoMode(640, 480), "test")
	, mWorld(mWindow)
{
}

void Game::run()
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


void Game::processEvents()
{
	sf::Event event;

	while (mWindow.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Game::update(sf::Time TimePerFrame)
{
	mWorld.update(TimePerFrame.asSeconds());
}

void Game::render()
{
	mWindow.clear();
	mWorld.draw();
	//mWindow.draw(mWorld);
	mWindow.display();
}
