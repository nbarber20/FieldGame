#pragma once
#include <string>
#include "TextDisplay.h"
#include "World.h"
#include "ObservationManager.h"
#include "DialogTree.h"
#include "Constants.h"
#include <iostream>
#include <sstream>
class InteractionManager
{
private:
	enum InteractionState
	{
		WorldInteraction,
		Dialog,
	};
public:
	InteractionManager(){};
	void Update(std::string input, TextDisplay* textdisplay);
	void ParsePlayerInput(std::string input, TextDisplay* textdisplay);
	std::string GetVerb(std::vector<std::string> splitparticles, Entity_Player* player);
	Entity* GetNoun(Entity_Player* player);
	Entity* GetAdjectiveSubject(std::string adj, std::string subject, Entity_Player* player);
	Position getPosition(std::vector<std::string> splitparticles);
	void EnterDialog(DialogTree* refTree);
	void EnterWorldInteraction();
	void LogDialog(TextDisplay* textdisplay);
private:
	InteractionState currentInteractionState = WorldInteraction;
	Constants constants;
	
	DialogTree* currentDialogTree;
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
		"north",
		"south",
		"east",
		"west",
		"all",
	};
};

