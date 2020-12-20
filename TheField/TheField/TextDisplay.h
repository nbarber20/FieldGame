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
		Log(std::string text, sf::Color c) {
			logText = text;
			logColor = c;
		}
		std::string logText = "";
		sf::Color logColor;
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
		Log newLog = Log("",sf::Color::Red);
		sf::Texture* tex = new sf::Texture();
		if (tex->loadFromFile(filename)) {
			newLog.tex = tex;
		}
		else {
			newLog.logText = "Image Error";
		}
		logs.push_back(newLog);
	}
	void addLog(Log newlog) {

		//std::transform(newlog.logText.begin(), newlog.logText.end(), newlog.logText.begin(), std::tolower);


		std::size_t found = newlog.logText.find_first_of(' ');
		while (found != std::string::npos && found<charLength)
		{
			found = newlog.logText.find_first_of(' ', found + 1);
		}
		if (found != std::string::npos) {

			Log firstLog = Log(newlog.logText.substr(0, found), newlog.logColor);
			Log secondLog = Log(newlog.logText.substr(found +1, newlog.logText.length()-1), newlog.logColor);

			logs.push_back(firstLog);
			addLog(secondLog);
		}
		else {
			logs.push_back(newlog);
		}
	}
private:
	std::vector<Log> logs;
	int charLength = 100;
};

