#include "pch.h"
#include "Entity_Player.h"
#include "ObservationManager.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "Entity_Room.h"
#include "Entity_Interior.h"
#include "Entity_Food.h"
#include "Entity_Event.h"
#include "World.h"

void Entity_Player::Tick()
{
	Entity_Living::Tick();
	std::vector<Entity*> inMouth = GetInventory(Mouth);
	for (auto object : inMouth)
	{
		TrySwallow(object);
	}

	if (nourishment <= 0) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are starving";
		ObservationManager::Instance().MakeObservation(o);
	}
	else if (nourishment <= 20) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You really need to eat";
		ObservationManager::Instance().MakeObservation(o);
	}
	else if (nourishment <= 40) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You could eat something";
		ObservationManager::Instance().MakeObservation(o);
	}

	if (hydration <= 0) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are dying of dehydration";
		ObservationManager::Instance().MakeObservation(o);
	}
	else if (hydration <= 20) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You really need to drink something";
		ObservationManager::Instance().MakeObservation(o);
	}
	else if (hydration <= 40) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are feeling thirsty";
		ObservationManager::Instance().MakeObservation(o);
	}
}

std::vector<Entity*> Entity_Player::getVisibleEntities(bool getParent)
{
	std::vector<Entity*> visible;
	std::vector<Entity*> onPerson = GetInventory();
	for (int i = 0; i < onPerson.size(); i++) {
		if (onPerson[i] != this) {
			visible.push_back(onPerson[i]);
			if (onPerson[i]->visibleInsides) {
				std::vector<Entity*> subEntity = onPerson[i]->GetInventory();
				for (int j = 0; j < subEntity.size(); j++) {
					visible.push_back(subEntity[j]);
					if (subEntity[j]->visibleInsides) {
						std::vector<Entity*> subEntity2 = subEntity[j]->GetInventory();
						for (int k = 0; k < subEntity2.size(); k++) {
							visible.push_back(subEntity2[k]);
						}
					}
				}
			}
		}
	}

	std::vector<Entity*> withintile = (parent.second)->GetInventory();
	for (int i = 0; i < withintile.size(); i++) {
		if (withintile[i] != this) {
			visible.push_back(withintile[i]);
			if (withintile[i]->visibleInsides) {
				std::vector<Entity*> subEntity = withintile[i]->GetInventory();
				for (int j = 0; j < subEntity.size(); j++) {
					visible.push_back(subEntity[j]);
					if (subEntity[j]->visibleInsides) {
						std::vector<Entity*> subEntity2 = subEntity[j]->GetInventory();
						for (int k = 0; k < subEntity2.size(); k++) {
							visible.push_back(subEntity2[k]);
						}
					}
				}
			}
		}
	}

	if (getParent) {
		Entity_Room* parentRoomTest = dynamic_cast<Entity_Room*>(parent.second);
		if (parentRoomTest) {
			visible.push_back(parentRoomTest);
			visible.push_back(parentRoomTest->parent.second);
		}
	}

	return visible;
}

void Entity_Player::CheckForEvents()
{

	std::vector<Entity*> nearbyEntities = getVisibleEntities(false);
	for (int i = 0; i < nearbyEntities.size();i++) {
		Entity_Event* checkEvent = dynamic_cast<Entity_Event*>(nearbyEntities[i]);
		if (checkEvent) {
			checkEvent->AttemptTrigger();
			return;
		}
	}
}


void Entity_Player::Look()
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities(false);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		if (nearbyEntities[i]->names.size() > 0) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Notice;
			o.referenceEntity = nearbyEntities[i];
			ObservationManager::Instance().MakeObservation(o);
		}
	}


	Entity_Room* roomTest = dynamic_cast<Entity_Room*>(parent.second);
	if (roomTest) {
		Entity_Interior* interiorTest = dynamic_cast<Entity_Interior*>(roomTest->parent.second);
		if (interiorTest) {
			for (int i = 0; i < 4; i++) {
				FacingDirection dir = (FacingDirection)i;
				Entity* room = interiorTest->GetAdjacent(dir, roomTest);
				if (room) {
					ObservationManager::Observation o = ObservationManager::Observation();
					o.sense = ObservationManager::SENSE_Look;
					o.type = ObservationManager::TYPE_Notice;
					o.referenceEntity = room;
					o.information = "There is a " + room->names[0] + " to the " + ObservationManager::Instance().FacingDirectionToString(dir);
					ObservationManager::Instance().MakeObservation(o);
				}
			}
		}
	}


}


void Entity_Player::Look(Entity* subject)
{
	if (subject->lookInfo != "") {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = subject;
		o.information = subject->lookInfo;
		ObservationManager::Instance().MakeObservation(o);
	}
	else {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = subject;
		o.information = "You find nothing of note";
		ObservationManager::Instance().MakeObservation(o);
	}
}

Entity* Entity_Player::FindEntityByName(std::string entityName)
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities(true);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		for (int n = 0; n < nearbyEntities[i]->names.size(); n++) {
			if (nearbyEntities[i]->names[n] == entityName) {
				return nearbyEntities[i];
			}
		}
	}
	return nullptr;
}
Entity* Entity_Player::FindEntityByName(std::string entityName, std::string adjective)
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities(true);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		for (int n = 0; n < nearbyEntities[i]->names.size(); n++) {
			if (nearbyEntities[i]->names[n] == entityName) {
				std::vector<std::string> ad = nearbyEntities[i]->GetAdjectives();
				for (int j = 0; j < ad.size(); j++) {
					if (ad[j] == adjective) {
						return nearbyEntities[i];
					}
				}
			}
		}
	}
	return nullptr;
}


Entity* Entity_Player::FindEntityByName(std::string entityName, std::string adjective, std::vector<Position> positionBlacklist)
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities(true);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		for (int n = 0; n < nearbyEntities[i]->names.size(); n++) {
			if (nearbyEntities[i]->names[n] == entityName) {
				std::vector<std::string> ad = nearbyEntities[i]->GetAdjectivesBlacklisted(positionBlacklist);
				for (int j = 0; j < ad.size(); j++) {
					if (ad[j] == adjective) {
						return nearbyEntities[i];
					}
				}
			}
		}
	}
	return nullptr;
}


Entity* Entity_Player::FindEntityByName(std::string entityName, Position realitivePosition, std::string realitiveEntityName)
{
	
	std::vector<Entity*> nearbyEntities = getVisibleEntities(true);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		for (int n = 0; n < nearbyEntities[i]->names.size(); n++) {
			if (nearbyEntities[i]->names[n] == entityName) {
				if (nearbyEntities[i]->parent.second != nullptr && nearbyEntities[i]->parent.first == realitivePosition) {
					if (realitiveEntityName == "") {
						return nearbyEntities[i];
					}
					for (int n2 = 0; n2 < nearbyEntities[i]->parent.second->names.size(); n2++) {
						if (nearbyEntities[i]->parent.second->names[n2] == realitiveEntityName) {
							return nearbyEntities[i];
						}
					}
				}
			}
		}
	}
	return nullptr;
}

bool Entity_Player::TryMove(Entity* e, Position toPos, Entity* toEntity)
{
	if (toPos == Nowhere) return false;
	if (toEntity == nullptr)return false;
	if (e->attachedToParent == true)return false;
	Entity_Room* roomTest = dynamic_cast<Entity_Room*>(toEntity);
	if (roomTest) {
		if (toPos == Inside)toPos = OnFloor;
		if (toPos == On)toPos = OnFloor;
	}
	if (toEntity->IsChildOf(e))return false;
	return e->SetParent(toPos, toEntity);
}

bool Entity_Player::Enter(Entity* e)
{
	return TryMove(this,Inside, e);
}

bool Entity_Player::Exit(Entity* e)
{
	Entity* target = e->parent.second;
	Entity_Interior* roomTest = dynamic_cast<Entity_Interior*>(e->parent.second);
	if (roomTest) {
		target = roomTest->parent.second;
	}
	if (target != nullptr) {
		if (target->internalVolume != 0) {
			return TryMove(this, Inside, target);
		}
		else {
			return TryMove(this, On, target);
		}
	}
	return false;
}

bool Entity_Player::Drink(Entity* e, bool drinkAll)
{
	Entity_Container* container = dynamic_cast<Entity_Container*>(e);
	if (container) {
		std::vector<Entity*> fluidCheck = container->GetInventory(Position::Inside);
		for (int i = 0; i < fluidCheck.size(); i++) {
			Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(fluidCheck[i]);
			if (fluid) {
				if (drinkAll) {
					if (fluid->size > constants.drinkableWaterThreshold) {
						ObservationManager::Observation o = ObservationManager::Observation();
						o.sense = ObservationManager::SENSE_Look;
						o.type = ObservationManager::TYPE_Direct;
						o.referenceEntity = e;
						o.information = "There is too much to drink";
						ObservationManager::Instance().MakeObservation(o);
						return true;
					}
					fluid->SetParent(Mouth, this);
				}
				else {
					Entity* e = fluid->SplitFluid(constants.mouthSize);
					e->SetParent(Mouth, this);
				}
			}
		}
		if (fluidCheck.size() == 0) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.referenceEntity = e;
			o.information = "The " + e->names[0] + " is empty";
			ObservationManager::Instance().MakeObservation(o);
		}
		return true;
	}
	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {
		
		if (drinkAll) {
			if (fluid->size > constants.drinkableWaterThreshold) {
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Look;
				o.type = ObservationManager::TYPE_Direct;
				o.referenceEntity = e;
				o.information = "There is too much to drink";
				ObservationManager::Instance().MakeObservation(o);
				return true;
			}
			fluid->SetParent(Mouth, this);
		}
		else {
			Entity* e = fluid->SplitFluid(constants.mouthSize);
			e->SetParent(Mouth, this);
		}
		return true;
	}
	return false;
}

bool Entity_Player::Eat(Entity* e)
{
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		e->SetParent(Mouth, this);
		return true;
	}
	return false;
}

bool Entity_Player::TrySwallow(Entity* e)
{

	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {
		if (fluid->swallowable) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Taste;
			o.type = ObservationManager::TYPE_Direct;
			o.information = "You drink the " + e->GetRandomAdjective(Taste)+ " " + e->names[0];
			ObservationManager::Instance().MakeObservation(o);
			this->AddHydration(fluid->hydration);
			World::Instance().RemoveEntity(fluid);
			return true;
		}
	}
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		if (food->spoiled == false) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Taste;
			o.type = ObservationManager::TYPE_Direct;
			o.information = "You eat the " + e->GetRandomAdjective(Taste) + " " + e->names[0];
			ObservationManager::Instance().MakeObservation(o);
			this->AddNourishment(food->nutritionalValue);
			World::Instance().RemoveEntity(food);
			return true;
		}
	}



	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Direct;
	o.referenceEntity = e;
	o.information = "You spit out the" + e->names[0];
	ObservationManager::Instance().MakeObservation(o);

	e->SetParent(parent.first, parent.second);
	return false;
}
