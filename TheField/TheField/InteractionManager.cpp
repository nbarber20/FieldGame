#include "pch.h"
#include "InteractionManager.h"
#include "Entity_Constructed.h"
#include "Entity_Container.h"
#include "Entity_Readable.h"
#include "Entity_Room.h"
#include "Entity_Interior.h"
#include "Entity_Npc.h"

void InteractionManager::Update(std::string input, TextDisplay* textdisplay)
{
	Entity_Player* player = World::Instance().playerEntity;
	if (currentInteractionState == WorldInteraction) {
		ParsePlayerInput(input, textdisplay, player);
		InputError inputError = AttemptPlayerCommand(textdisplay,player);
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
			LogDialog(textdisplay);
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

	verb = GetVerb(player);
	subject = GetNoun(player);
	predicate = nullptr;	
	if (subject != nullptr) {
		predicate = GetNoun(player);
	}
}

InteractionManager::InputError InteractionManager::AttemptPlayerCommand(TextDisplay* textdisplay,Entity_Player* player)
{
	if (verb == "look") {
		player->Look();
		return Success;
	}
	if (verb == "rotate") {
		if (subject) {
			subject->FaceClockWise();
			return Success;
		}
		return NeedsSubject;
	}
	if (verb == "godir") {
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
	if (verb == "enter") {
		if (subject) {
			if (player->Enter(subject)) {
				player->Look();
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	if (verb == "exit") {
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
	if (verb == "flip") {
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
	if (verb == "take") {
		if (subject) {
			if (player->TryMove(subject, Position::RightHand, player)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	if (verb == "drink") {
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
	if (verb == "eat") {
		if (subject) {
			if (player->Eat(subject)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	if (verb == "break") {
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
	if (verb == "pour") {
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
	if (verb == "read") {
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
	if (verb == "put") {
		if (subject&&predicate) {
			Position putPos = getPosition();
			if (player->TryMove(subject, putPos, predicate)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubjectPredicate;
	}
	if (verb == "drop") {
		if (subject) {
			if (player->TryMove(subject, player->parent.first, player->parent.second)) {
				return Success;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	if (verb == "talk") {
		if (subject) {
			Entity_Npc* npc = dynamic_cast<Entity_Npc*>(subject);
			if (npc) {
				if (npc->dialogTree != nullptr) {
					EnterDialog(npc->dialogTree);
					LogDialog(textdisplay);
					return Success;
				}
				return Impossible;
			}
			return Impossible;
		}
		return NeedsSubject;
	}
	if (verb == "goddelete") {
		if (subject) {
			World::Instance().RemoveEntity(subject);
		}
		return Success;
	}
	if (verb == "wait") {
		textdisplay->addLog(TextDisplay::Log("You wait", sf::Color::Red));
		return Success;
	}
	if (verb == "") {
		return Success;
	}
}

std::string InteractionManager::GetVerb(Entity_Player* player)
{
	if (splitInput[0] == "put")
	{
		if (FindParticleInput("down")) {
			return "drop";
		}
		else {
			return "put";
		}
	}
	if (splitInput[0] == "go") {
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
	if (splitInput[0] == "get") {
		if (FindParticleInput("in")) {
			return "enter";
		}
		if (FindParticleInput("out")) {
			return "exit";
		}
	}

	if (splitInput[0] == "look"|| splitInput[0] == "examine") {
		return "look";
	}
	if (splitInput[0] == "rotate" || splitInput[0] == "turn") {
		return "rotate";
	}
	if (splitInput[0] == "enter") {
		return "enter";
	}
	if (splitInput[0] == "leave" || splitInput[0] == "exit") {
		return "exit";
	}
	if (splitInput[0] == "flip") {
		return "flip";
	}
	if (splitInput[0] == "take" || splitInput[0] == "steal"|| splitInput[0] == "pickup" || splitInput[0] == "get") {
		return "take";
	}
	if (splitInput[0] == "drink" || splitInput[0] == "sip") {
		return "drink";
	}
	if (splitInput[0] == "eat" || splitInput[0] == "nibble") {
		return "eat";
	}
	if (splitInput[0] == "break") {
		return "break";
	}
	if (splitInput[0] == "pour") {
		return "pour";
	}
	if (splitInput[0] == "read") {
		return "read";
	}
	if (splitInput[0] == "drop") {
		return "drop";
	}
	if (splitInput[0] == "talk" || splitInput[0] == "speak") {
		return "talk";
	}
	if (splitInput[0] == "wait") {
		return "wait";
	}

	//GodMode is enabled
	if (constants.godMode) {
		if (splitInput[0] == "goddelete") {
			return "goddelete";
		}
	}

	return " ";
}

Entity* InteractionManager::GetNoun(Entity_Player* player)
{
	Entity* subject = nullptr;
	std::string toRemove[2];
	int i = 0;
	do {
		subject = player->FindEntityByName(splitInput[i]);
		toRemove[0] = splitInput[i];
		if (subject == nullptr&& i< splitInput.size()-1) {
			subject = GetAdjectiveSubject(splitInput[i], splitInput[i + 1], player);
			toRemove[0] = splitInput[i];
			toRemove[1] = splitInput[i + 1];
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
	if(FindParticleInput("on")) {
		return On;
	}
	return Inside;
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

void InteractionManager::LogDialog(TextDisplay* textdisplay)
{
	textdisplay->addLog(TextDisplay::Log(currentDialogTree->TreeNodes[currentDialogTree->currentIndex].dialog, sf::Color::Yellow));
	for (int i = 0; i < currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses.size() + 1; i++) {

		std::string c = "a";
		if (i == 1) c = "b";
		if (i == 2) c = "c";
		if (i == 3) c = "d";
		if (i < currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses.size()) {
			std::string s = c + "): " + currentDialogTree->TreeNodes[currentDialogTree->currentIndex].responses[i].first;
			textdisplay->addLog(TextDisplay::Log(s, sf::Color::Yellow));
		}
		else {
			std::string s = c + "): Leave";
			textdisplay->addLog(TextDisplay::Log(s, sf::Color::Yellow));
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

