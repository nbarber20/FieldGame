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
		ParsePlayerInput(input, textdisplay);
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

void InteractionManager::ParsePlayerInput(std::string input, TextDisplay* textdisplay)
{
	splitInput.clear();
	std::vector<std::string> splitparticles;
	int i = 0;
	std::stringstream ssin(input);
	while (ssin.good()) {
		std::string temp;
		ssin >> temp;
		if (std::find(particles.begin(), particles.end(), temp) == particles.end()){
			splitInput.push_back(temp);
			i++;
		}
		else {
			splitparticles.push_back(temp);
		}
	}

	textdisplay->addLog(TextDisplay::Log(input, sf::Color::Red));

	Entity_Player* player = World::Instance().playerEntity;
	std::string verb = GetVerb(splitparticles, player);
	Entity* subject = GetNoun(player);
	Entity* predicate = nullptr;	
	if (subject != nullptr) {
		predicate = GetNoun(player);
	}

	if (verb == "look") {
		player->Look();
		return;
	}
	if (verb == "rotate") {
		if (subject) {
			subject->FaceClockWise();
			return;
		}
		textdisplay->addLog(TextDisplay::Log("Rotate what?", sf::Color::Red));
		return;
	}
	if (verb == "godir") {
		Entity_Room* roomtest = dynamic_cast<Entity_Room*>(player->parent.second);
		if (roomtest) {
			Entity_Interior* interorTest = dynamic_cast<Entity_Interior*>(roomtest->parent.second);
			if (interorTest) {
				if (std::find(splitparticles.begin(), splitparticles.end(), "north") != splitparticles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(North, roomtest));
					player->Look();
					return;
				}
				else if (std::find(splitparticles.begin(), splitparticles.end(), "south") != splitparticles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(South, roomtest));
					player->Look();
					return;
				}
				else if (std::find(splitparticles.begin(), splitparticles.end(), "east") != splitparticles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(East, roomtest));
					player->Look();
					return;
				}
				else if (std::find(splitparticles.begin(), splitparticles.end(), "west") != splitparticles.end()) {
					player->TryMove(player, OnFloor, interorTest->GetAdjacent(West, roomtest));
					player->Look();
					return;
				}
			}
		}
		textdisplay->addLog(TextDisplay::Log("Which direction?", sf::Color::Red));
		return;
	}
	if (verb == "enter") {
		if (subject) {
			if (player->Enter(subject) == true) {
				player->Look();
				return;
			}
		}
		textdisplay->addLog(TextDisplay::Log("You try as you might, but you just cant go there", sf::Color::Red));
		return;
	}
	if (verb == "exit") {
		if (subject) {
			if (player->Exit(subject) == true) {
				player->Look();
				return;
			}
		}
		else {
			if (player->Exit(player->parent.second) == true) {
				player->Look();
				return;
			}
		}
		textdisplay->addLog(TextDisplay::Log("You try as you might, but you just cant leave", sf::Color::Red));
		return;
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
			return;
		}
		textdisplay->addLog(TextDisplay::Log("Flip what?", sf::Color::Red));
		return;
	}
	if (verb == "take") {
		if (subject) {
			player->TryMove(subject, Position::RightHand, player);
			return;
		}
		textdisplay->addLog(TextDisplay::Log("Take what?", sf::Color::Red));
		return;
	}
	if (verb == "drink") {
		if (subject) {
			bool drinkall = false;
			if (std::find(splitparticles.begin(), splitparticles.end(), "all") != splitparticles.end()) {
				drinkall = true;
			}
			if (player->Drink(subject, drinkall) == false) {
				textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
			}
			return;
		}
		textdisplay->addLog(TextDisplay::Log("Drink what?", sf::Color::Red));
		return;
	}
	if (verb == "eat") {
		if (subject) {
			if (player->Eat(subject) == false) {
				textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
			}
			return;
		}
		textdisplay->addLog(TextDisplay::Log("Eat what?", sf::Color::Red));
		return;
	}
	if (verb == "break") {
		if (subject) {
			Entity_Constructed* constructed = dynamic_cast<Entity_Constructed*>(subject);
			if (constructed) {
				constructed->BreakConstructed(player->strength);
				return; 
			}
		}
		textdisplay->addLog(TextDisplay::Log("You cant break it", sf::Color::Red));
		return;
	}
	if (verb == "pour") {
		if (subject) {
			Entity_Container* container = dynamic_cast<Entity_Container*>(subject);
			if (container) {
				if (predicate) {
					if (container->PourInto(predicate) == false) {
						textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
					}
					return;
				}
				else {
					if (container->PourInto(container->parent.second) == false) {
						textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
					}
					return;
				}
			}
			else {
				Entity_Container* container2 = dynamic_cast<Entity_Container*>(subject->parent.second);
				if (container2) {
					if (predicate) {
						if (container2->PourInto(predicate) == false) {
							textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
						}
						return;
					}
					else {
						if (container2->PourInto(container2->parent.second) == false) {
							textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
						}
						return;
					}
				}
			}
		}
		textdisplay->addLog(TextDisplay::Log("You cant", sf::Color::Red));
		return;
	}
	if (verb == "read") {
		if (subject) {
			Entity_Readable* readable = dynamic_cast<Entity_Readable*>(subject);
			if (readable) {
				readable->Read(player);
				return;
			}
		}
		textdisplay->addLog(TextDisplay::Log("Read what?", sf::Color::Red));
		return;
	}
	if (verb == "put") {
		if (subject&&predicate) {
			Position putPos = getPosition(splitparticles);
			if (player->TryMove(subject,putPos,predicate)==false){
				textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
			}
		}
		return;
	}
	if (verb == "drop") {
		if (subject) {
			if (player->TryMove(subject, player->parent.first, player->parent.second) == false) {
				textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
			}
			return;
		}
		textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
		return;
	}
	if (verb == "talk") {
		if (subject) {
			Entity_Npc* npc = dynamic_cast<Entity_Npc*>(subject);
			if (npc) {
				if (npc->dialogTree != nullptr) {
					EnterDialog(npc->dialogTree);
					LogDialog(textdisplay);
					return;
				}
				textdisplay->addLog(TextDisplay::Log("They have nothing to say", sf::Color::Red));
				return;
			}
		}
		textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
		return;
	}
	if (verb == "goddelete") {
		if (subject) {
			World::Instance().RemoveEntity(subject);
			return;
		}
		textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
		return;
	}
	if (verb == "wait") {
		textdisplay->addLog(TextDisplay::Log("You wait", sf::Color::Red));
		return;
	}
	if (verb == " ") {
		textdisplay->addLog(TextDisplay::Log("You cant do that", sf::Color::Red));
		return;
	}
}

std::string InteractionManager::GetVerb(std::vector<std::string> unused, Entity_Player* player)
{
	if (splitInput[0] == "put")
	{
		if (std::find(unused.begin(), unused.end(), "down") != unused.end()) {
			return "drop";
		}
		else {
			return "put";
		}
	}
	if (splitInput[0] == "go") {
		if (std::find(unused.begin(), unused.end(), "in") != unused.end()) {
			return "enter";
		}
		if (std::find(unused.begin(), unused.end(), "outside") != unused.end()) {
			return "exit";
		}
		if (std::find(unused.begin(), unused.end(), "out") != unused.end()) {
			return "exit";
		}
		if (std::find(unused.begin(), unused.end(), "into") != unused.end()) {
			return "exit";
		}
		if (std::find(unused.begin(), unused.end(), "inside") != unused.end()) {
			return "enter";
		}
		if (std::find(unused.begin(), unused.end(), "north") != unused.end()) {
			return "godir";
		}
		if (std::find(unused.begin(), unused.end(), "south") != unused.end()) {
			return "godir";
		}
		if (std::find(unused.begin(), unused.end(), "east") != unused.end()) {
			return "godir";
		}
		if (std::find(unused.begin(), unused.end(), "west") != unused.end()) {
			return "godir";
		}
	}
	if (splitInput[0] == "get") {
		if (std::find(unused.begin(), unused.end(), "in") != unused.end()) {
			return "enter";
		}
		if (std::find(unused.begin(), unused.end(), "out") != unused.end()) {
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

Position InteractionManager::getPosition(std::vector<std::string> unused)
{
	if (std::find(unused.begin(), unused.end(), "front") != unused.end()) {
		return InFront;
	}
	if (std::find(unused.begin(), unused.end(), "behind") != unused.end()) {
		return Behind;
	}
	if (std::find(unused.begin(), unused.end(), "below") != unused.end()) {
		return Below;
	}
	if (std::find(unused.begin(), unused.end(), "top") != unused.end()) {
		return On;
	}
	if (std::find(unused.begin(), unused.end(), "in") != unused.end()) {
		return Inside;
	}
	if (std::find(unused.begin(), unused.end(), "outside") != unused.end()) {
		return InFront;
	}
	if (std::find(unused.begin(), unused.end(), "out") != unused.end()) {
		return InFront;
	}
	if (std::find(unused.begin(), unused.end(), "into") != unused.end()) {
		return Inside;
	}
	if (std::find(unused.begin(), unused.end(), "inside") != unused.end()) {
		return Inside;
	}
	if (std::find(unused.begin(), unused.end(), "on") != unused.end()) {
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

