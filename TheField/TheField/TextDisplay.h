#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
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
	};

	TextDisplay() {};
	std::string getVisibleText() {
		std::string text;

		for (int i = 0; i < logs.size(); i++) {
			text += logs[i].logText + '\n';
		}
		return text;
	}
	void addLog(Log newlog) {
		logs.push_back(newlog);
	}
private:
	std::vector<Log> logs;
};

