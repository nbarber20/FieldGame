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
		MainMenu,
		Intro,
		WorldInteraction,
		Dialog,
		SaveOverride,
		LoadOverride,
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
	InputError AttemptPlayerCommand(Entity_Player* player);
	std::string GetHelp(std::string HelpPage);
	std::string GetVerb(Entity_Player* player,int index);
	Entity* GetNoun(Entity_Player* player);
	Entity* GetAdjectiveSubject(std::string adj, std::string subject, Entity_Player* player);
	Entity* GetPositionalSubject(std::string subject, std::string relevant,  Entity_Player* player);
	Position getPosition();
	void EnterDialog(DialogTree* refTree);
	void EnterWorldInteraction();
	void LogDialog();
	bool FindParticleInput(std::string toFind);
private:
	InteractionState currentInteractionState = SaveOverride;
	int introStage = 0;
	int mainMenuStage = 0;
	Constants constants;
	
	std::string lastInput;
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
		"floor",
		"north",
		"south",
		"east",
		"west",
		"all",
		"self",
	};
};

