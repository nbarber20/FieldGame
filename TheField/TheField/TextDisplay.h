#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <string>
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

		for (auto log : logs){
			text += log.logText + '\n';
		}
		return text;
	}
	void addLog(Log newlog) {
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

