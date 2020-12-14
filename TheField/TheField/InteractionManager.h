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
	enum InputError
	{
		Success,
		NeedsSubject,
		NeedsPredicate,
		NeedsSubjectPredicate,
		Impossible,
	};
public:
	InteractionManager(){};
	void Update(std::string input, TextDisplay* textdisplay);
	void ParsePlayerInput(std::string input, TextDisplay* textdisplay, Entity_Player* player);
	InputError AttemptPlayerCommand(TextDisplay* textdisplay, Entity_Player* player);
	std::string GetVerb(Entity_Player* player);
	Entity* GetNoun(Entity_Player* player);
	Entity* GetAdjectiveSubject(std::string adj, std::string subject, Entity_Player* player);
	Position getPosition();
	void EnterDialog(DialogTree* refTree);
	void EnterWorldInteraction();
	void LogDialog(TextDisplay* textdisplay);
	bool FindParticleInput(std::string toFind);
private:
	InteractionState currentInteractionState = WorldInteraction;
	Constants constants;
	
	std::string verb;
	Entity* subject;
	Entity* predicate; 
	std::vector<std::string> particles;



	DialogTree* currentDialogTree;
	std::vector <std::string> splitInput;
	std::vector<std::string> particleList = {
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

