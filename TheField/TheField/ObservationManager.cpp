#include "pch.h"
#include "ObservationManager.h"
#include "TextDisplay.h"
#include <SFML/Graphics.hpp>
#include "Entity.h"

void ObservationManager::CompileObservations(Entity* playerEntity, TextDisplay* textDisplay)
{
	Constants constants;

	ObservationType lastType = TYPE_All; 
	for (int i = 0; i < observations.size(); i++) {
		
		Observation o = observations[i]; 
		if (o.displayed == true)continue;
		o.displayed = true;



		if (o.type == TYPE_Notice && lastType != TYPE_Notice&& o.information == "") {
			textDisplay->addLog(TextDisplay::Log("You see: ", sf::Color::Yellow));

		}

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
		std::string depthTab= "";


		if (o.depth > 0) {
			for (int i = 0; i < o.depth; i++) {
				depthTab += "   ";
			}
			depthTab += "-";
		}

		if (e != nullptr) {
			if (e->names.size() > 0) {

				for (int j = 0; j < observations.size(); j++) {
					if (observations[j].displayed == false) {
						if (o.sense == observations[j].sense && o.type == observations[j].type &&  observations[j].referenceEntity != nullptr) {
							if (observations[j].referenceEntity != e) {
								if (e->parent == observations[j].referenceEntity->parent && e->countable == true&& e->individualName =="")
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
				if (e->countable&& plural == false && e->individualName == "") {
					indefdefrefName = "a " + e->names[0];
					indefdefrefNameADJ = "a " + ADJ + e->names[0];
				}
				else {
					indefdefrefName = e->names[0];
					indefdefrefNameADJ = ADJ + e->names[0];
				}
			}
		}


		if (o.type == TYPE_Image) {
			textDisplay->addImage(o.imageFile);
		}
		if (o.type == TYPE_Direct) {
			textDisplay->addLog(TextDisplay::Log(o.information,sf::Color::Yellow));
		}
		if (o.type == TYPE_Notice) {
			if (o.information != "")
			{
				textDisplay->addLog(TextDisplay::Log(o.information, sf::Color::Yellow));
			}
			else {
				if (e->parent.second == playerEntity){
					textDisplay->addLog(TextDisplay::Log(depthTab+ indefdefrefNameADJ + " " + preposition + " your " + GetComponentName(e->parent.first), sf::Color::Yellow));
				}
				else {
					if (plural) {
						if (e->size > constants.smallItemMaxThreshold && e->parent.second == playerEntity->parent.second) {
							textDisplay->addLog(TextDisplay::Log(depthTab + numberStrings[pluralCount] + " " + indefdefrefNameADJ + "s", sf::Color::Yellow));							
						}
						else {
							if (prepositionNoun) {
								textDisplay->addLog(TextDisplay::Log(depthTab + numberStrings[pluralCount] + " " + indefdefrefNameADJ + "s " + preposition, sf::Color::Yellow));
							}
							else {
								textDisplay->addLog(TextDisplay::Log(depthTab + numberStrings[pluralCount] + " " + indefdefrefNameADJ + "s " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
							}
						}
					}
					else {
						if (e->size > constants.smallItemMaxThreshold && e->parent.second == playerEntity->parent.second) {
							textDisplay->addLog(TextDisplay::Log(depthTab + indefdefrefNameADJ, sf::Color::Yellow));
						}
						else {
							if (prepositionNoun) {
								textDisplay->addLog(TextDisplay::Log(depthTab + indefdefrefNameADJ + " " + preposition, sf::Color::Yellow));
							}
							else {
								textDisplay->addLog(TextDisplay::Log(depthTab + indefdefrefNameADJ + " " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
							}
						}
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
							textDisplay->addLog(TextDisplay::Log("You are outside", sf::Color::Yellow));

						}
						else {
							textDisplay->addLog(TextDisplay::Log("You are " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
						}
					}
					else {
						textDisplay->addLog(TextDisplay::Log("You are in the " + e->parent.second->names[0], sf::Color::Yellow));
					}
				}
				else {
					if (prepositionNoun) {
						textDisplay->addLog(TextDisplay::Log(defrefNameADJ+ " is now " + preposition, sf::Color::Yellow));
					}
					else {
						if (e->parent.second != nullptr) {
							textDisplay->addLog(TextDisplay::Log(defrefNameADJ + " is now " + preposition + " the " + e->parent.second->names[0], sf::Color::Yellow));
						}
					}
				}
			}
		}
		if (o.type == TYPE_Rotation) {
			textDisplay->addLog(TextDisplay::Log(defrefNameADJ + " is now " + RotationToString(e->rotation), sf::Color::Yellow));
		}
		if (o.type == TYPE_FacingDirection) {
			textDisplay->addLog(TextDisplay::Log(defrefNameADJ + " is now facing " + FacingDirectionToString(e->facingDirection), sf::Color::Yellow));
		}
		lastType = o.type;
	}
}


void ObservationManager::ConsumeObservations(std::vector<std::pair<int, int>> toConsume)
{
	for (int i = 0; i < toConsume.size(); i++) {
		ObservationType type = (ObservationType)toConsume[i].first;
		ObservationSense sense = (ObservationSense)toConsume[i].second;
		if (type == TYPE_All&& sense == TYPE_All) {
			ClearObservations();
			return;
		}

		for (int j = 0; j < observations.size(); j++) {
			if (type == TYPE_All) {
				if (observations[j].sense == sense) {
					RemoveObservation(j);
				}
			}
			else if (sense == SENSE_All) {
				if (observations[j].type == type) {
					RemoveObservation(j);
				}
			}
			else {
				if (observations[j].type == type && observations[j].sense == sense) {
					RemoveObservation(j);
				}
			}
		}
	}
}

void ObservationManager::RemoveObservationForEntity(Entity* entity)
{
	for (int i = 0; i < observations.size(); i++) {
		if (observations[i].referenceEntity == entity) {
			RemoveObservation(i);
			return;
		}
	}
}

void ObservationManager::RemoveObservation(int index)
{
	observations.erase(std::remove(observations.begin(), observations.end(), observations[index]), observations.end());
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
	return "ERROR";
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
	return "ERROR";
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
	case Back:
		return individualName + "'s back";
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
	return "ERROR";
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
	case Back:
		return "back";
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
	return "ERROR";
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
	case Back:
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
	return "ERROR";
}


