#include <pch.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "ObservationManager.h"
#include <World.h>
#include <GameLoader.h>
#include "LevelEditor.h"
#pragma warning(disable : 4996)
int main() {
	Constants constants;

	//Setup window
	sf::Event event;
	float w = sf::VideoMode::getDesktopMode().width * .5f;
	float h = sf::VideoMode::getDesktopMode().height * .5f;
	sf::RenderWindow window(sf::VideoMode(w, h), "The Fields Level Editor");
	window.setFramerateLimit(30);
	sf::View mainView(sf::FloatRect(0.0f, 0.0f, 800., 800.f));
	float windowRatio = w / h;
	float viewRatio = 1;
	float sizeX = 1;
	float sizeY = 1;
	float posX = 0;
	float posY = 0;
	if (windowRatio > viewRatio) {
		sizeX = viewRatio / windowRatio;
		posX = (1 - sizeX) / 2.f;
	}
	else {
		sizeY = windowRatio / viewRatio;
		posY = (1 - sizeY) / 2.f;
	}
	mainView.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
	window.setView(mainView);


	//Setup Text and Sprite
	TextDisplay* textDisplay = new TextDisplay();
	sf::Font font;
	if (!font.loadFromFile(GameLoader::Instance().GetDirectory() + "Data/GameData/dogicapixel.ttf"))
	{
		MessageBox(nullptr, TEXT("Data/GameData/dogicapixel.ttf is missing!"), TEXT("Error"), MB_OK);
		return -1;
	}


	sf::Text consoleText;
	consoleText.setFont(font);
	consoleText.setCharacterSize(16);
	consoleText.setLineSpacing(2);
	consoleText.setPosition(0, 16);
	consoleText.setFillColor(constants.pallette2);

	sf::Sprite consoleSprite = sf::Sprite();
	consoleSprite.setPosition(0, 0);
	consoleSprite.setScale(4, 4);

	ObservationManager::Instance().ClearObservations();
	textDisplay->addImage("Data/Art/Title.png");
	textDisplay->addLog("");
	textDisplay->addLog("enter new, load, or delete");
	textDisplay->addLog("");


	LevelEditor levelEditor;
	GameLoader::Instance().exitOnFailure = false;
	levelEditor.SetupNew();

	
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{

				sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
				sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
				levelEditor.ClickEditor(worldPos);
			}
			//Entry
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r')
				{
					levelEditor.InputCharacter(event.text.unicode);
				}
			}
			//Backspace
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::BackSpace) {
					levelEditor.RemoveCharacter();
				}
			}
			//Input command
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				levelEditor.SaveInputData();
			}
			
			//Scroll
			if (event.type == sf::Event::MouseWheelMoved) {
				if (event.mouseWheel.delta > 0) {
					levelEditor.scrollOffset += 48;
				}
				else {
					levelEditor.scrollOffset += -48;
				}
			}
			//Exit
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::Resized) {
				float windowRatio = event.size.width / (float)event.size.height;
				float sizeX = 1;
				float sizeY = 1;
				float posX = 0;
				float posY = 0;
				if (windowRatio > 1) {
					sizeX = 1 / windowRatio;
					posX = (1 - sizeX) / 2.f;
				}
				else {
					sizeY = windowRatio / 1;
					posY = (1 - sizeY) / 2.f;
				}
				mainView.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
			}
		}
		window.setView(mainView);
		window.clear(constants.pallette1);
		levelEditor.DrawEditor(&consoleText, &consoleSprite, &window);		
		window.display();
	}
	return 0;
}