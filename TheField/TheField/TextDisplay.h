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

	void DisplayText(float scrollPos, sf::Text* displayTex, sf::Sprite* sprite, sf::RenderWindow* window)
	{

		int yOffset = 0;
		for (int i = 0; i < logs.size();i++) {
			if (logs[i].tex != nullptr) {
				sprite->setTexture(*logs[i].tex);
				sprite->setPosition(200,scrollPos + yOffset);
				window->draw(*sprite);
				yOffset += 160;
			}
			else {
				displayTex->setPosition(0, scrollPos + yOffset);
				displayTex->setString(logs[i].logText);
				window->draw(*displayTex);
				yOffset += 20;
			}
		}
	}


	void addImage(std::string filename) {
		Log newLog = Log("");
		sf::Texture* tex = new sf::Texture();
		if (tex->loadFromFile(filename)) {
			newLog.tex = tex;
		}
		else {
			newLog.logText = "Image Error";
		}
		logs.push_back(newLog);
	}
	void addLog(std::string newlog) {
		std::size_t found = newlog.find_first_of(' ');
		while (found != std::string::npos && found<charLength)
		{
			found = newlog.find_first_of(' ', found + 1);
		}		
		if (found != std::string::npos && found >charLength) {

			Log firstLog = Log(newlog.substr(0, found));
			Log secondLog = Log(newlog.substr(found +1, newlog.length()-1));
			logs.push_back(firstLog);
			addLog(secondLog.logText);
		}
		else {
			logs.push_back(newlog);
		}
	}
private:
	std::vector<Log> logs;
	int charLength = 50;
};

