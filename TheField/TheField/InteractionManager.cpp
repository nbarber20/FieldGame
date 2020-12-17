#include "pch.h"
#include "InteractionManager.h"
#include "Entity_Constructed.h"
#include "Entity_Container.h"
#include "Entity_Readable.h"
#include "Entity_Room.h"
#include "Entity_Interior.h"
#include "Entity_Npc.h"
#include "Entity_Mechanisim.h"
#include "Entity_Firearm.h"

void InteractionManager::Update(std::string input, TextDisplay* textdisplay)
{
	Entity_Player* player = World::Instance().playerEntity;

	if (currentInteractionState == Intro) {
		introStage++;
		if (introStage == 1) {
			textdisplay->addLog(TextDisplay::Log(input, sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("How did you die?", sf::Color::Yellow));
		}
		else if (introStage == 2) {
			textdisplay->addLog(TextDisplay::Log(input, sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("", sf::Color::Yellow));
			textdisplay->addImage("Data/Field_Start.png");
			textdisplay->addLog(TextDisplay::Log("You find yourself laying in a large grassy field. Its nearly sunset", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("Perhaps this is a dream", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("Perhaps this is a the afterlife", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("What will you do next?", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("", sf::Color::Yellow));
			currentInteractionState = WorldInteraction;
		}

	}
	else if (currentInteractionState == WorldInteraction) {
		if (input == "again")
		{
			ParsePlayerInput(lastInput, textdisplay, player);
		}
		else {
			lastInput = input;
			ParsePlayerInput(input, textdisplay, player);
		}
		InputError inputError = AttemptPlayerCommand(player);
		if (inputError == NeedsSubject) {
			textdisplay->addLog(TextDisplay::Log("Specify a subject", sf::Color::Red));
		}
		else if (inputError == NeedsPredicate) {
			textdisplay->addLog(TextDisplay::Log("Specify a target", sf::Color::Red));
		}
		else if (inputError == NeedsSubjectPredicate) {
			textdisplay->addLog(TextDisplay::Log("Specify a subject and target", sf::Color::Red));
		}
		else if (inputError == Impossible) {
			textdisplay->addLog(TextDisplay::Log("That isn't currently possible", sf::Color::Red));
		}


		player->CheckForEvents();
		std::vector<TextDisplay::Log> observations = ObservationManager::Instance().CompileObservations(player);
		for (int i = 0; i < observations.size(); i++) {

			textdisplay->addLog(observations[i]);
		}
		ObservationManager::Instance().ClearObservations();
		World::Instance().Tick();
		observations = ObservationManager::Instance().CompileObservations(player);
		for (int i = 0; i < observations.size(); i++) {

			textdisplay->addLog(observations[i]);
		}
		ObservationManager::Instance().ClearObservations();
		textdisplay->addLog(TextDisplay::Log("", sf::Color::Red));
	}
	else if (currentInteractionState == Dialog) {

		DialogTree::DialogTreeEvent event = DialogTree::EVENT_NONE;

		if (input == "a")event = currentDialogTree->Respond(0);
		if (input == "b")event = currentDialogTree->Respond(1);
		if (input == "c")event = currentDialogTree->Respond(2);
		if (input == "d")event = currentDialogTree->Respond(3);
		if (event == DialogTree::EVENT_NONE) {
			LogDialog();
			std::vector<TextDisplay::Log> observations = ObservationManager::Instance().CompileObservations(player);
			for (int i = 0; i < observations.size(); i++) {

				textdisplay->addLog(observations[i]);
			}
			ObservationManager::Instance().ClearObservations();
		}
		else {
			EnterWorldInteraction();
			textdisplay->addLog(TextDisplay::Log("...", sf::Color::Yellow));
			textdisplay->addLog(TextDisplay::Log("", sf::Color::Yellow));
		}
	}
}

void InteractionManager::ParsePlayerInput(std::string input, TextDisplay* textdisplay, Entity_Player* player)
{
	splitInput.clear();
	particles.clear();
	int i = 0;
	std::stringstream ssin(input);
	while (ssin.good()) {
		std::string temp;
		ssin >> temp;
		if (std::find(particleList.begin(), particleList.end(), temp) == particleList.end()){
			splitInput.push_back(temp);
			i++;
		}
		else {
			particles.push_back(temp);
		}
	}

	textdisplay->addLog(TextDisplay::Log(input, sf::Color::Red));

	verb = GetVerb(player,0);
	subject = GetNoun(player);
	predicate = nullptr;	
	if (subject != nullptr) {
		predicate = GetNoun(player);
	}
}

InteractionManager::InputError InteractionManager::AttemptPlayerCommand(Entity_Player* player)
{
	if (verb == "look") {
		if (subject) {
			player->Look(subject);
			return Success;
		}
		player->Look();
		return Success;
	}
	else if (verb == "rotate") {
		if (subject) {
			subject->FaceClockWise();
			return Success;
		}
		return NeedsSubject;
	}
	else if (verb == "godir") {
		Entity_Room* roomtest = dynamic_cast<Entity_Room*>(player->parent.second);
		if (roomtest) {
			Entity_Interior* interorTest = dynamic_cast<Entity_Interior*>(roomtest->parent.second);
			if (interorTest) {
				if (std::find(particles.begin(), particles.end(), "north") != particles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(North, roomtest));
					player->Look();
					return Success;
				}
				else if (std::find(particles.begin(), particles.end(), "south") != particles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(South, roomtest));
					player->Look();
					return Success;
				}
				else if (std::find(particles.begin(), particles.end(), "east") != particles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(East, roomtest));
					player->Look();
					return Success;
				}
				else if (std::find(particles.begin(), particles.end(), "west") != particles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(West, roomtest));
					player->Look();
					return Success;
				}
			}
		}
		return Impossible;
	}
	else if (verb == "enter") {
		if (subject) {
			if (player->Enter(subject)) {
				player->Look();
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "exit") {
		if (subject) {
			if (player->Exit(subject)) {
				player->Look();
				return Success;
			}
		}
		else {
			if (player->Exit(player->parent.second)) {
				player->Look();
				return Success;
			}
		}
		return Impossible;
	}
	else if (verb == "flip") {
		if (subject) {
			if (subject->rotation == Upright) {
				subject->Rotate(UpsideDown);
			}
			else if (subject->rotation == UpsideDown) {
				subject->Rotate(Upright);
			}
			else {
				subject->Rotate(UpsideDown);
			}
			return Success;
		}
		return NeedsSubject;
	}
	else if (verb == "take") {
		if (subject) {
			if (player->TryMove(subject, Position::RightHand, player)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "drink") {
		if (subject) {
			bool drinkall = false;
			if (std::find(particles.begin(), particles.end(), "all") != particles.end()) {
				drinkall = true;
			}
			if (player->Drink(subject, drinkall)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "eat") {
		if (subject) {
			if (player->Eat(subject)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "break") {
		if (subject) {
			Entity_Constructed* constructed = dynamic_cast<Entity_Constructed*>(subject);
			if (constructed) {
				if (constructed->BreakConstructed(player->strength)) {
					return Success;
				}
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "pour") {
		if (subject) {
			Entity_Container* container = dynamic_cast<Entity_Container*>(subject);
			if (container) {
				if (predicate) {
					if (container->PourInto(predicate)) {
						return Success;
					}
				}
				else {
					if (container->PourInto(container->parent.second)) {
						return Success;
					}
				}
				return Impossible;
			}
			else {
				Entity_Container* container2 = dynamic_cast<Entity_Container*>(subject->parent.second);
				if (container2) {
					if (predicate) {
						if (container2->PourInto(predicate)) {
							return Success;
						}
					}
					else {
						if (container2->PourInto(container2->parent.second)) {
							return Success;
						}
					}
				}
				return Impossible;
			}
		}
		return NeedsSubject;
	}
	else if (verb == "read") {
		if (subject) {
			Entity_Readable* readable = dynamic_cast<Entity_Readable*>(subject);
			if (readable) {
				readable->Read(player);
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "put") {
		if (subject&&predicate) {
			Position putPos = getPosition();
			if (player->TryMove(subject, putPos, predicate)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubjectPredicate;
	}
	else if (verb == "drop") {
		if (subject) {
			if (player->TryMove(subject, player->parent.first, player->parent.second)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "talk") {
		if (subject) {
			Entity_Npc* npc = dynamic_cast<Entity_Npc*>(subject);
			if (npc) {
				if (npc->dialogTree != nullptr) {
					EnterDialog(npc->dialogTree);
					LogDialog();
					return Success;
				}
				return Impossible;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	else if (verb == "goddelete") {
		if (subject) {
			World::Instance().RemoveEntity(subject);
		}
		return Success;
	}
	else if (verb == "wait") {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You wait";
		ObservationManager::Instance().MakeObservation(o);
		return Success;
	}
	else if (verb == "help") {

		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = GetHelp(GetVerb(player, 1));
		ObservationManager::Instance().MakeObservation(o);
		return Success;
	}
	else if (verb == "fire") {
		if (subject&& predicate) {
			Entity_Firearm* gun = dynamic_cast<Entity_Firearm*>(subject);
			if (gun) {
				gun->Fire(predicate);
				return Success;
			}
			return Impossible;
		}
		return NeedsSubjectPredicate;
	}
	else{
		if (subject) {
			Entity_Mechanisim* mechanism = dynamic_cast<Entity_Mechanisim*>(subject);
			if (mechanism) {
				if (mechanism->AttemptBehavior(verb,predicate)) {
					return Success;
				}
			}
			return Impossible;
		}
		return Success;
	}
}

std::string InteractionManager::GetHelp(std::string HelpPage)
{
	if (HelpPage == "") {
		return "Commands should be a verb, followed by a subject. Some commands also require a subject and target. Try out some of the following commands: look, enter, go (direction), talk, take, drop, put";
	}
	//TODO add more help pages.
}

std::string InteractionManager::GetVerb(Entity_Player* player, int index)
{
	if (index < 0 || index >= splitInput.size())return "";

	if (splitInput[index] == "put")
	{
		if (FindParticleInput("down")) {
			return "drop";
		}
		else {
			return "put";
		}
	}
	if (splitInput[index] == "go") {
		if (FindParticleInput("in")) {
			return "enter";
		}
		if (FindParticleInput("outside")) {
			return "exit";
		}
		if (FindParticleInput("out")) {
			return "exit";
		}
		if (FindParticleInput("into")) {
			return "enter";
		}
		if (FindParticleInput("inside")) {
			return "enter";
		}
		if (FindParticleInput("north")) {
			return "godir";
		}
		if (FindParticleInput("south")) {
			return "godir";
		}
		if (FindParticleInput("east")) {
			return "godir";
		}
		if (FindParticleInput("west")) {
			return "godir";
		}
	}
	if (splitInput[index] == "get") {
		if (FindParticleInput("in")) {
			return "enter";
		}
		if (FindParticleInput("out")) {
			return "exit";
		}
	}

	if (splitInput[index] == "look"|| splitInput[index] == "examine" || splitInput[index] == "peek" || splitInput[index] == "study" || splitInput[index] == "watch") {
		return "look";
	}
	if (splitInput[index] == "rotate" || splitInput[index] == "turn" || splitInput[index] == "spin") {
		return "rotate";
	}
	if (splitInput[index] == "enter") {
		return "enter";
	}
	if (splitInput[index] == "leave" || splitInput[index] == "exit") {
		return "exit";
	}
	if (splitInput[index] == "flip"|| splitInput[index] == "overturn" || splitInput[index] == "roll") {
		return "flip";
	}
	if (splitInput[index] == "take" || splitInput[index] == "steal"|| splitInput[index] == "pickup" || splitInput[index] == "get" || splitInput[index] == "remove") {
		return "take";
	}
	if (splitInput[index] == "drink" || splitInput[index] == "sip") {
		return "drink";
	}
	if (splitInput[index] == "eat" || splitInput[index] == "nibble" || splitInput[index] == "consume" || splitInput[index] == "ingest") {
		return "eat";
	}
	if (splitInput[index] == "break") {
		return "break";
	}
	if (splitInput[index] == "pour") {
		return "pour";
	}
	if (splitInput[index] == "read") {
		return "read";
	}
	if (splitInput[index] == "drop") {
		return "drop";
	}
	if (splitInput[index] == "talk" || splitInput[index] == "speak") {
		return "talk";
	}
	if (splitInput[index] == "fire" || splitInput[index] == "shoot") {
		return "fire";
	}
	if (splitInput[index] == "wait") {
		return "wait";
	}
	if (splitInput[index] == "help") {
		return "help";
	}

	//GodMode is enabled
	if (constants.godMode) {
		if (splitInput[index] == "goddelete") {
			return "goddelete";
		}
	}

	return splitInput[index];
}

Entity* InteractionManager::GetNoun(Entity_Player* player)
{
	Entity* subject = nullptr;
	std::string toRemove[2];
	int i = 0;
	do {
		if (i < splitInput.size()) {
			toRemove[0] = splitInput[i];
			if (subject == nullptr) {
				subject = GetPositionalSubject(splitInput[i], "", player);
			}
			if (subject == nullptr&& i < splitInput.size() - 1) {
				subject = GetPositionalSubject(splitInput[i], splitInput[i + 1], player);
				toRemove[1] = splitInput[i + 1];
			}
			if (subject == nullptr) {
				subject = player->FindEntityByName(splitInput[i]);
			}
			if (subject == nullptr&& i < splitInput.size() - 1) {
				subject = GetAdjectiveSubject(splitInput[i], splitInput[i + 1], player);
				toRemove[1] = splitInput[i + 1];
			}
		}
		i++;

	} while (subject == nullptr&& i< splitInput.size());
	splitInput.erase(std::remove(splitInput.begin(), splitInput.end(), toRemove[0]), splitInput.end());
	splitInput.erase(std::remove(splitInput.begin(), splitInput.end(), toRemove[1]), splitInput.end());
	return subject;
}

Entity* InteractionManager::GetAdjectiveSubject(std::string adj, std::string subject, Entity_Player* player)
{
	Entity* found = player->FindEntityByName(subject, adj, { {Position::Taste} });
	if (found != nullptr) {
		//null subject if it cant actually be seen.
		if (found->visibleInsides == false) {
			if (player->FindEntityByName(subject, adj, { {Position::Taste,Position::Inside} }) == nullptr) {
				found = nullptr;
			}
		}
	}
	return found;
}

Entity* InteractionManager::GetPositionalSubject(std::string subject, std::string relevant, Entity_Player* player)
{
	Position p = getPosition();
	if (p == Nowhere)return nullptr;
	return player->FindEntityByName(subject,p,relevant);
}

Position InteractionManager::getPosition()
{
	if (FindParticleInput("front")) {
		return InFront;
	}
	if (FindParticleInput("behind")) {
		return Behind;
	}
	if (FindParticleInput("below")) {
		return Below;
	}
	if (FindParticleInput("top")) {
		return On;
	}
	if (FindParticleInput("in")) {
		return Inside;
	}
	if (FindParticleInput("outside")) {
		return InFront;
	}
	if (FindParticleInput("out")) {
		return InFront;
	}
	if (FindParticleInput("into")) {
		return Inside;
	}
	if (FindParticleInput("inside")) {
		return Inside;
	}
	if (FindParticleInput("on")&& FindParticleInput("floor")) {
		return OnFloor;
	}
	if(FindParticleInput("on")) {
		return On;
	}
	return Nowhere;
}

void InteractionManager::EnterDialog(DialogTree* refTree)
{
	currentInteractionState = Dialog;
	currentDialogTree = refTree;
	
}

void InteractionManager::EnterWorldInteraction()
{
	currentInteractionState = WorldInteraction;
}

void InteractionManager::LogDialog()
{
	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Direct;
	o.information = currentDialogTree->TreeNodes[currentDialogTree->currentIndex].dialog;
	ObservationManager::Instance().MakeObservation(o);
	for (int i = 0; i < currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses.size() + 1; i++) {

		std::string c = "a";
		if (i == 1) c = "b";
		if (i == 2) c = "c";
		if (i == 3) c = "d";
		if (i < currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses.size()) {
			std::string s = c + "): " + currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses[i].first;
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.information = s;
			ObservationManager::Instance().MakeObservation(o);
		}
		else {
			std::string s = c + "): Leave";
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.information = s;
			ObservationManager::Instance().MakeObservation(o);
		}
	}

}

bool InteractionManager::FindParticleInput(std::string toFind)
{
	if (std::find(particles.begin(), particles.end(), toFind) != particles.end()) {
		return true;
	}
	return false;
}

