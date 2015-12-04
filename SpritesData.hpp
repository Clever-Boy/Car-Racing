#pragma once


#include <SFML/Graphics/Rect.hpp>

#include <vector>


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

};