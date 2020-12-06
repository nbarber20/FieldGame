#include "pch.h"
#include "ObservationManager.h"
#include "TextDisplay.h"
#include <SFML/Graphics.hpp>
#include "Entity.h"

std::vector <TextDisplay::Log> ObservationManager::CompileObservations(Entity* playerEntity)
{
	std::vector <TextDisplay::Log> logs;
	Constants constants;


	for (int i = 0; i < observations.size(); i++) {
		
		Observation o = observations[i]; 
		if (o.displayed == true)continue;
		o.displayed = true;
		Entity* e = o.referenceEntity;

		std::string defrefName="";
		std::string indefdefrefName = "";
		std::string defrefNameADJ = "";
		std::string indefdefrefNameADJ = "";
		std::string ADJ = "";
		std::string preposition = "";
		bool prepositionNoun=false;
		bool plural = false;
		int pluralCount = 0;

		if (e != nullptr) {

			for (int j = 0; j < observations.size(); j++) {
				if (observations[j].displayed == false) {
					if(o.sense == observations[j].sense && o.type == observations[j].type &&  observations[j].referenceEntity != nullptr){
						if (observations[j].referenceEntity != e) {
							if (e->parent == observations[j].referenceEntity->parent)
							{
								if (e->CheckforNameMatch(observations[j].referenceEntity))
								{
									observations[j].displayed = true;
									plural = true;
									pluralCount++;
								}
							}
						}
					}
				}
			}

			preposition = getPreposition(e->parent.first, &prepositionNoun);
			ADJ = e->GetRandomAdjective(Visual) + " ";
			if (ADJ == " ")ADJ = "";
			defrefName = "the " + e->names[0];
			defrefNameADJ = "the " + ADJ + e->names[0];
			if (e->coutable&& plural == false) {
				indefdefrefName = "a " + e->names[0];
				indefdefrefNameADJ = "a " + ADJ + e->names[0];
			}
			else {
				indefdefrefName = e->names[0];
				indefdefrefNameADJ = ADJ + e->names[0];
			}
		}



		if (o.type == TYPE_Direct) {
			logs.push_back(TextDisplay::Log(o.information,sf::Color::Yellow));
		}
		if (o.type == TYPE_Notice) {
			if (e->parent.second == playerEntity) 
			{
				logs.push_back(TextDisplay::Log("There is " + indefdefrefNameADJ + " " + preposition + " your " + GetComponentName(e->parent.first), sf::Color::Yellow));
			}
			else if (e->parent == playerEntity->parent) {
				if (e->size < constants.smallItemMaxThreshold) {
					if (prepositionNoun) {
						logs.push_back(TextDisplay::Log("There is " + indefdefrefNameADJ + " " + preposition, sf::Color::Yellow));
					}
					else {
						logs.push_back(TextDisplay::Log("There is " + indefdefrefNameADJ + " " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
					}
				}
				else {
					logs.push_back(TextDisplay::Log("You see " + indefdefrefNameADJ, sf::Color::Yellow));
				}
			}
			else {
				if (plural) {
					if (prepositionNoun) {
						logs.push_back(TextDisplay::Log("There are " + numberStrings[pluralCount] + " " + indefdefrefName + "s " + preposition, sf::Color::Yellow));
					}
					else {
						logs.push_back(TextDisplay::Log("There are " + numberStrings[pluralCount] + " " + indefdefrefName+ "s " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
					}
				}
				else {
					if (prepositionNoun) {
						logs.push_back(TextDisplay::Log("There is " + indefdefrefNameADJ + " " + preposition, sf::Color::Yellow));
					}
					else {
						logs.push_back(TextDisplay::Log("There is " + indefdefrefNameADJ + " " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
					}
				}

			}
		}
		if (o.type == TYPE_Movement) {
			if (e->parent.second != playerEntity) {
				if (e == playerEntity) {
					if (!prepositionNoun) {
						bool wasinside = o.lastState == "inside" || o.lastState == "on the floor";
						if (wasinside && e->parent.first != Inside) {
							logs.push_back(TextDisplay::Log("You are outside", sf::Color::Yellow));

						}
						else {
							logs.push_back(TextDisplay::Log("You are " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
						}
					}
				}
				else {
					if (prepositionNoun) {
						logs.push_back(TextDisplay::Log(defrefName + " is now " + preposition, sf::Color::Yellow));
					}
					else {
						if (e->parent.second != nullptr) {
							logs.push_back(TextDisplay::Log(defrefName + " is now " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
						}
					}
				}
			}
		}
		if (o.type == TYPE_Rotation) {
			logs.push_back(TextDisplay::Log(defrefName + " is now " + RotationToString(e->rotation), sf::Color::Yellow));
		}
		if (o.type == TYPE_FacingDirection) {
			logs.push_back(TextDisplay::Log(defrefName + " is now facing " + FacingDirectionToString(e->facingDirection), sf::Color::Yellow));
		}

	}
	return logs;
}


std::string ObservationManager::RotationToString(Rotation r)
{
	switch (r)
	{
	case Upright:
		return "upright";
	case UpsideDown:
		return "upsideDown";
	case Tipped:
		return "tipped";
	}
}

std::string ObservationManager::FacingDirectionToString(FacingDirection r)
{
	switch (r)
	{
	case North:
		return "north";
	case East:
		return "east";
	case South:
		return "south";
	case West:
		return "west";
	}
}

std::string ObservationManager::PositionToString(Position r, std::string individualName)
{
	switch (r)
	{
	case InFront:
		return "in front";
	case Inside:
		return "inside";
	case Behind:
		return "behind";
	case On:
		return "on";
	case Below:
		return "below";
	case RightHand:
		return individualName + "'s right hand";
	case LeftHand:
		return individualName + "'s left hand";
	case Arms:
		return individualName + "'s arms";
	case Chest:
		return individualName + "'s chest";
	case Legs:
		return individualName + "'s legs";
	case Feet:
		return individualName + "'s feet";
	case Head:
		return individualName + "'s head";
	case Mouth:
		return individualName + "'s mouth";
	case OnWall:
		return "wall";
	case OnFloor:
		return "floor";
	case OnCieling:
		return "ceiling";
	case Visual:
		return "ERROR";
	case Taste:
		return "ERROR";
	case Smell:
		return "ERROR";
	}
}

std::string ObservationManager::GetComponentName(Position pos)
{
	switch (pos)
	{
	case InFront:
		return "ERROR";
	case Inside:
		return "ERROR";
	case Behind:
		return "ERROR";
	case On:
		return "ERROR";
	case Below:
		return "ERROR";
	case RightHand:
		return "right hand";
	case LeftHand:
		return "left hand";
	case Arms:
		return "arms";
	case Chest:
		return "chest";
	case Legs:
		return "legs";
	case Feet:
		return "feet";
	case Head:
		return "head";
	case Mouth:
		return "mouth";
	case OnWall:
		return "wall";
	case OnFloor:
		return "floor";
	case OnCieling:
		return "ceiling";
	case Visual:
		return "ERROR";
	case Taste:
		return "ERROR";
	case Smell:
		return "ERROR";
	}
}

std::string ObservationManager::getPreposition(Position pos, bool* containsNoun)
{
	*containsNoun = false;
	switch (pos)
	{
	case InFront:
		return "in front of";
	case Inside:
		return "in";
	case Behind:
		return "behind";
	case On:
		return "on";
	case Below:
		return "below";
	case RightHand:
		return "in";
	case LeftHand:
		return "in";
	case Arms:
		return "on";
	case Chest:
		return "on";
	case Legs:
		return "on";
	case Feet:
		return "on";
	case Head:
		return "on";
	case Mouth:
		return "in";
	case OnWall:
		*containsNoun = true;
		return "on the wall";
	case OnFloor:
		*containsNoun = true;
		return "on the floor";
	case OnCieling:
		*containsNoun = true;
		return "on the ceiling";
	case Visual:
		return "ERROR";
	case Taste:
		return "ERROR";
	case Smell:
		return "ERROR";
	}
}

