#include "pch.h"
#include "Entity_Player.h"
#include "ObservationManager.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "Entity_Room.h"

std::vector<Entity*> Entity_Player::getVisibleEntities()
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

	return visible;
}

Entity* Entity_Player::getNearbyEntity(std::string entityName)
{
	Entity* target;
	std::vector<Entity*> nearbyEntities = getVisibleEntities();
	for (int i = 0; i < nearbyEntities.size(); i++) {
		for (int n = 0; n < nearbyEntities[i]->names.size(); n++) {
			if (nearbyEntities[i]->names[n] == entityName) {
				return nearbyEntities[i];
			}
		}
		if (nearbyEntities[i]->individualName == entityName) {
			return nearbyEntities[i];
		}
	}
	return nullptr;
}

void Entity_Player::Look()
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities();
	for (int i = 0; i < nearbyEntities.size(); i++) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Notice;
		o.referenceEntity = nearbyEntities[i];
		o.information = "there is a " + nearbyEntities[i]->names[0] + " here";
		ObservationManager::Instance().MakeObservation(o);
	}
}


Entity* Entity_Player::FindEntityByName(std::string entityName)
{
	std::vector<Entity*> nearbyEntities = getVisibleEntities();
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
	std::vector<Entity*> nearbyEntities = getVisibleEntities();
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
	std::vector<Entity*> nearbyEntities = getVisibleEntities();
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


bool Entity_Player::TryMove(Entity* e, Position toPos, Entity* toEntity)
{
	if (e->attachedToParent == true)return;
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
	return this->SetParent(Inside, e);
}

bool Entity_Player::Exit(Entity* e)
{
	if (e->parent.second->GetInventory(OnFloor).size() > 0) { //we can assume this is a house or something with a floor.
		return this->SetParent(OnFloor, e->parent.second);
	}
	else {
		if (e->parent.second->internalVolume!=0) {
			return this->SetParent(Inside, e->parent.second);
		}
		else {
			return this->SetParent(On, e->parent.second);
		}
	}
}

bool Entity_Player::Drink(Entity* e)
{
	Entity_Container* container = dynamic_cast<Entity_Container*>(e);
	if (container) {
		std::vector<Entity*> fluidCheck = container->GetInventory(Position::Inside);
		for (int i = 0; i < fluidCheck.size(); i++) {
			Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(fluidCheck[i]);
			if (fluid) {
				fluid->SetParent(Mouth, this);
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
		fluid->SetParent(Mouth, this);
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
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.referenceEntity = e;
			o.information = "You drink the " + e->GetRandomAdjective(Taste)+ " " + e->names[0];
			ObservationManager::Instance().MakeObservation(o);

			e->SetParent(Inside, nullptr);
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
