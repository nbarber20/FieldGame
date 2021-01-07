#include "pch.h"
#include "ObservationManager.h"
#include "TextDisplay.h"
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Entity_Room.h"
void ObservationManager::CompileObservations(Entity* playerEntity, TextDisplay* textDisplay)
{
	Constants constants;

	Observation* lastObservation = nullptr;
	for (auto & observation : observations) {		
		
		if (observation->displayed == true)continue;
		observation->displayed = true;

		//Headers
		Observation_Look* observation_OldLook = dynamic_cast<Observation_Look*>(lastObservation);
		Observation_Look* observation_NewLook = dynamic_cast<Observation_Look*>(observation);
		if (observation_NewLook && observation_OldLook == nullptr) {
			textDisplay->addLog("You see: "); 
		}

		lastObservation = observation;



		std::string depthTab = "";
		if (observation->depth > 0) {
			for (int i = 0; i < observations[i]->depth; i++) {
				depthTab += "   ";
			}
			depthTab += "-";
		}

		Observation_Image* observation_Image = dynamic_cast<Observation_Image*>(observation);
		if (observation_Image) {
			textDisplay->addImage(observation_Image->imageFile);
			continue;
		}

		Observation_Direct* observation_Direct = dynamic_cast<Observation_Direct*>(observation);
		if (observation_Direct) {
			textDisplay->addLog(observation_Direct->text);
			continue;
		}

		Observation_Look* observation_Look = dynamic_cast<Observation_Look*>(observation);
		if (observation_Look) {
			if (observation_Look->directional) {
				textDisplay->addLog(GetIndefNameAndParticle(observation_Look->referenceEntity) + " to the " + FacingDirectionToString(observation_Look->direction));
			}
			else {
				if (observation_Look->depth > 0 || observation_Look->referenceEntity->size < constants.smallItemMaxThreshold)
				{
					bool containsNoun;
					std::string preposition = getPreposition(observation_Look->referenceEntity->parent.first, &containsNoun, observation_Look->referenceEntity->parent.second == playerEntity);
					if (containsNoun) {
						textDisplay->addLog(depthTab + GetIndefNameAndParticle(observation_Look->referenceEntity) + " " + preposition);
					}
					else {
						textDisplay->addLog(depthTab + GetIndefNameAndParticle(observation_Look->referenceEntity) + " " + preposition + " " + GetNameAndParticle(observation_Look->referenceEntity->parent.second));
					}
				}
				else {
					textDisplay->addLog(depthTab + GetIndefNameAndParticle(observation_Look->referenceEntity));
				}
			}
			continue;
		}

		Observation_Movement* observation_Movement = dynamic_cast<Observation_Movement*>(observation);
		if (observation_Movement) {
			bool containsNoun;
			std::string preposition = getPreposition(observation_Movement->referenceEntity->parent.first, &containsNoun, observation_Movement->referenceEntity == playerEntity);

			Entity_Room* isLastRoom = dynamic_cast<Entity_Room*>(observation_Movement->lastParent);
			Entity_Room* isInRoom = dynamic_cast<Entity_Room*>(observation_Movement->referenceEntity->parent.second);
			if (observation_Movement->lastParent->IsChildOf(observation_Movement->referenceEntity->parent.second) == false) {
				if (isLastRoom && isInRoom == nullptr) {
					containsNoun = true;
					preposition = "outside";
				}
				else if (isLastRoom == nullptr && isInRoom) {
					containsNoun = true;
					preposition = "inside";
				}
			}

			if (observation_Movement->referenceEntity == playerEntity) {
				if (containsNoun) {
					textDisplay->addLog("you are now " + preposition);
				}
				else {
					textDisplay->addLog("you are now " + preposition + " " +  GetNameAndParticle(observation_Movement->referenceEntity->parent.second));
				}
			}
			else {
				if (containsNoun) {
					textDisplay->addLog(GetNameAndParticle(observation_Movement->referenceEntity) + " is now " + preposition);
				}
				else {
					textDisplay->addLog(GetNameAndParticle(observation_Movement->referenceEntity) + " is now " + preposition + " " + GetNameAndParticle(observation_Movement->referenceEntity->parent.second));
				}
			}
		}

		Observation_Action* observation_Action = dynamic_cast<Observation_Action*>(observation);
		if (observation_Action) {
			if (observation_Action->referenceEntity == playerEntity) {
				textDisplay->addLog("you " + observation_Action->SecPersonverb + " " + GetNameAndParticle(observation_Action->target));
			}
			else {
				textDisplay->addLog(GetNameAndParticle(observation_Action->referenceEntity) + " " + observation_Action->ThirdPersonverb + " " + GetNameAndParticle(observation_Action->target));
			}
			continue;
		}

		Observation_Status* observation_Status = dynamic_cast<Observation_Status*>(observation);
		if (observation_Status) {
			if (observation_Status->referenceEntity == playerEntity) {
				textDisplay->addLog("you are " + observation_Status->statusString);
			}
			else {
				textDisplay->addLog(GetNameAndParticle(observation_Status->referenceEntity) + " is " + observation_Status->statusString);
			}
		}

		Observation_Sound* observation_Sound = dynamic_cast<Observation_Sound*>(observation);
		if (observation_Sound) {
			//TODO get vision?
			if (observation_Status->referenceEntity->IsChildOf(playerEntity->parent.second)) {
				//Player can see entity
				if (StartsWithVowel(observation_Sound->sound)) {
					textDisplay->addLog(GetNameAndParticle(observation_Status->referenceEntity) + " makes an " + observation_Sound->sound + " sound");
				}
				else {
					textDisplay->addLog(GetNameAndParticle(observation_Status->referenceEntity) + " makes a " + observation_Sound->sound + " sound");
				}
			}
			else {
				if (StartsWithVowel(observation_Sound->sound)) {
					textDisplay->addLog("You hear an " + observation_Sound->sound + " sound");
				}
				else {
					textDisplay->addLog("You hear a " + observation_Sound->sound + " sound");
				}

			}
		}
	}
}


void ObservationManager::RemoveObservationForEntity(Entity* entity)
{
	for (int i = 0; i < observations.size(); i++) {
		if (observations[i]->referenceEntity == entity) {
			RemoveObservation(i);
			return;
		}
	}
}

bool ObservationManager::StartsWithVowel(std::string input)
{
	if (input.length() == 0) return false;
	std::string vowels = "aeiou";
	for (char vowel : vowels) {
		if (vowel == input.at(0)) {
			return true;
		}
	}
	return false;
}

int ObservationManager::RandomRange(int start, int end)
{
	return (start + (std::rand() % ((end-1) - start + 1)));
}

std::string ObservationManager::GetNameAndParticle(Entity* e)
{

	if (e == nullptr)return "";
	if (e->names.empty())return "";

	std::vector<std::string> adjs = e->GetAdjectivesBlacklisted({ Taste });
	if (adjs.empty())
	{
		if (e->countable) {
			return	"the " + e->names[0];
		}
		else {
			return e->names[0];
		}
	}
	else {
		if (e->countable) {
			return "the " + adjs[RandomRange(0, adjs.size())] + " " + e->names[0];
		}
		else {
			return adjs[RandomRange(0, adjs.size())] + " " + e->names[0];
		}
	}
}

std::string ObservationManager::GetIndefNameAndParticle(Entity* e)
{
	if (e == nullptr)return "";
	if (e->names.empty())return "";
	std::vector<std::string> adjs = e->GetAdjectivesBlacklisted({ Taste });
	if (e->countable) {
		if (adjs.empty()) {
			if (StartsWithVowel(e->names[0])) {
				return "an " + e->names[0];
			}
			else {
				return "a " + e->names[0];
			}
		}
		else {
			std::string adj = adjs[RandomRange(0, adjs.size())];
			if (StartsWithVowel(adj)) {
				return "an " + adj + " " + e->names[0];
			}
			else {
				return "a " + adj + " " + e->names[0];
			}
		}
	}
	else {
		if (adjs.empty()) {
			return e->names[0];
		}
		else {
			return adjs[RandomRange(0, adjs.size())] + " " + e->names[0];
		}
	}
}

void ObservationManager::RemoveObservation(int index)
{
	//delete observations[index];
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
		return "tipped over";
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

std::string ObservationManager::getPreposition(Position pos, bool* containsNoun, bool isPlayer)
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
		*containsNoun = true;
		if (isPlayer) {
			return "in your right hand";
		}
		else {
			return "in their right hand";
		}
	case LeftHand:
		*containsNoun = true;
		if (isPlayer) {
			return "in your left hand";
		}
		else {
			return "in their left hand";
		}
	case Arms:
		*containsNoun = true;
		if (isPlayer) {
			return "on your arms";
		}
		else {
			return "on their arms";
		}
	case Back:
		*containsNoun = true;
		if (isPlayer) {
			return "on your back";
		}
		else {
			return "on their back";
		}
	case Chest:
		*containsNoun = true;
		if (isPlayer) {
			return "on your chest";
		}
		else {
			return "on their chest";
		}
	case Legs:
		*containsNoun = true;
		if (isPlayer) {
			return "on your legs";
		}
		else {
			return "on their legs";
		}
	case Feet:
		*containsNoun = true;
		if (isPlayer) {
			return "on your feet";
		}
		else {
			return "on their feet";
		}
	case Head:
		*containsNoun = true;
		if (isPlayer) {
			return "on your head";
		}
		else {
			return "on their head";
		}
	case Mouth:
		*containsNoun = true;
		if (isPlayer) {
			return "in your mouth";
		}
		else {
			return "in their mouth";
		}
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


