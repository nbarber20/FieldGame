#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <string>
#include <cctype>
class TextDisplay
{
public:

	struct Log
	{
		Log(std::string text) {
			logText = text;
		}
		std::string logText = "";
		sf::Texture* tex = nullptr;
	};
	TextDisplay() {};
	void DisplayText(float scrollPos, sf::Text* displayTex, sf::Sprite* sprite, sf::RenderWindow* window);
	void addImage(std::string filename);
	void addLog(std::string newlog);
private:
	std::vector<Log> logs;
	int charLength = 50;
};

