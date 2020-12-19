#pragma once
#include <SFML/Graphics.hpp>
class Constants {
public:
	Constants() {};
	const int smallItemMaxThreshold = 16;
	const int RoomMinThreshold = 10000;
	const float mouthSize = 7.5f;
	const float drinkableWaterThreshold = 231;
	const bool godMode = true;
	const float daySpeed = 3.0f;
	sf::Color pallette1 = sf::Color(8,20,30);
	sf::Color pallette2 = sf::Color(246, 214, 189);
	sf::Color pallette3 = sf::Color(90, 185, 168);
	sf::Color pallette4 = sf::Color(196, 240, 194);
};