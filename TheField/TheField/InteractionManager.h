#pragma once
#include <string>
#include "TextDisplay.h"
#include "World.h"
#include "ObservationManager.h"
#include <iostream>
#include <sstream>
class InteractionManager
{
public:
	InteractionManager(World* worldref) 
	{
		this->world = worldref;
	};
	void ParsePlayerInput(std::string input, TextDisplay* textdisplay);
	std::string GetVerb(std::vector<std::string> splitparticles, Entity_Player* player);
	Entity* GetNoun(Entity_Player* player);
	Entity* GetAdjectiveSubject(std::string adj, std::string subject, Entity_Player* player);
	Position getPosition(std::vector<std::string> splitparticles);
private:

	std::vector <std::string> splitInput;
	std::vector<std::string> particles = {
		"the",
		"a",
		"to",
		"fucking",

		"in",
		"inside",
		"other",
		"out",
		"next",
		"on",
		"front",
		"behind",
		"below",
		"top",

	};
	World* world;
};

