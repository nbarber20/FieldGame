#include "pch.h"
#include "Entity_Player.h"
#include "ObservationManager.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "Entity_Room.h"
#include "Entity_Interior.h"
#include "Entity_Food.h"
#include "Entity_Event.h"
#include "Entity_Npc.h"
#include "Entity_GroundTile.h"
#include "World.h"

void Entity_Player::Tick()
{
	Entity_Living::Tick();
}




void Entity_Player::CheckForEvents()
{

	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true,false,false);
	for (int i = 0; i < nearbyEntities.size();i++) {
		Entity_Event* checkEvent = dynamic_cast<Entity_Event*>(nearbyEntities[i]);
		if (checkEvent) {
			if (checkEvent->GetChildDepth() == GetChildDepth()) {
				checkEvent->AttemptTrigger();
				return;
			}
		}
	}
}


void Entity_Player::Look()
{
	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true,false, false);
	int playerdepth = GetChildDepth();
	for (int i = 0; i < nearbyEntities.size(); i++) {
		if (nearbyEntities[i]->names.size() > 0) {
			ObservationManager::Instance().MakeObservation(new Observation_Look(nearbyEntities[i], nearbyEntities[i]->GetChildDepth() - playerdepth));
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
					ObservationManager::Instance().MakeObservation(new Observation_Look(dir,room));
				}
			}
		}
	}
	Entity_GroundTile* groundTest = dynamic_cast<Entity_GroundTile*>(parent.second);
	if (groundTest) {

		if (groundTest->toNorth.second != -1)
		{
			ObservationManager::Instance().MakeObservation(new Observation_Direct(groundTest->toNorth.first,nullptr));
		}
		if (groundTest->toEast.second != -1)
		{
			ObservationManager::Instance().MakeObservation(new Observation_Direct(groundTest->toEast.first, nullptr));
		}
		if (groundTest->toSouth.second != -1)
		{
			ObservationManager::Instance().MakeObservation(new Observation_Direct(groundTest->toSouth.first, nullptr));
		}
		if (groundTest->toWest.second != -1)
		{
			ObservationManager::Instance().MakeObservation(new Observation_Direct(groundTest->toWest.first, nullptr));
		}
		
	}

}


void Entity_Player::Look(Entity* subject)
{
	if (subject->lookInfo != "") {
		ObservationManager::Instance().MakeObservation(new Observation_Direct(subject->lookInfo,subject));
	}
	else {
		ObservationManager::Instance().MakeObservation(new Observation_Direct("You find nothing of note", subject));
	}
}

void Entity_Player::LookSelf()
{
	int playerdepth = GetChildDepth();
	std::vector<Entity*> nearbyEntities = GetVisibleEntities(false, false, true);
	for (int i = 0; i < nearbyEntities.size(); i++) {
		if (nearbyEntities[i]->names.size() > 0) {
			ObservationManager::Instance().MakeObservation(new Observation_Look(nearbyEntities[i], nearbyEntities[i]->GetChildDepth() - playerdepth));
		}
	}
}

Entity* Entity_Player::FindEntityByName(std::string entityName)
{
	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true, true, true);
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
	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true, true, true);
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
	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true, true, true);
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

	std::vector<Entity*> nearbyEntities = GetVisibleEntities(true, true, true);
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
	if (e->weight > strength && e != this)return false;
	Entity* npcCheck = nullptr;
	if (e->IsChildOf(12, &npcCheck)){
		Entity_Npc* npc = dynamic_cast<Entity_Npc*>(npcCheck);
		npc->NoticePickpocket(this);
		return true;
	}

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

