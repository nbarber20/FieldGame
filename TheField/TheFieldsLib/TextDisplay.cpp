#include "pch.h"
#include "TextDisplay.h"
#include "GameLoader.h"

void TextDisplay::DisplayText(float scrollPos, sf::Text* displayTex, sf::Sprite* sprite, sf::RenderWindow* window)
{

	int yOffset = 0;
	for (auto & log : logs) {
		if (log.tex != nullptr) {
			sprite->setTexture(*log.tex);
			sprite->setPosition(200, scrollPos + yOffset);
			window->draw(*sprite);
			yOffset += 160;
		}
		else {
			displayTex->setPosition(0, scrollPos + yOffset);
			displayTex->setString(log.logText);
			window->draw(*displayTex);
			yOffset += 20;
		}
	}
}

void TextDisplay::addImage(std::string filename)
{
	Log newLog = Log("");
	sf::Texture* tex = new sf::Texture();
	if (tex->loadFromFile(GameLoader::Instance().GetDirectory() + filename)) {
		newLog.tex = tex;
	}
	else {
		newLog.logText = "Image Error";
	}
	logs.push_back(newLog);
}

void TextDisplay::addLog(std::string newlog)
{
	std::size_t found = newlog.find_first_of(' ');
	while (found != std::string::npos && found < charLength)
	{
		found = newlog.find_first_of(' ', found + 1);
	}
	if (found != std::string::npos && found > charLength) {

		Log firstLog = Log(newlog.substr(0, found));
		Log secondLog = Log(newlog.substr(found + 1, newlog.length() - 1));
		logs.push_back(firstLog);
		addLog(secondLog.logText);
	}
	else {
		logs.push_back(newlog);
	}
}
