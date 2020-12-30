#include "pch.h"
#include "World.h"
#include "GameLoader.h"
#include "ObservationManager.h"

void World::ClearEntities()
{
	entities.clear();
}

int World::AddEntity(Entity* e)
{
	entities.push_back(e);
	return entities.size()-1;
}

void World::MarkRemoveEntity(Entity* e)
{
	entitiesToDelete.push_back(e);
}

void World::RemoveMarkedEntities()
{
	for (auto e : entitiesToDelete) {
		RemoveEntity(e);
	}
	entitiesToDelete.clear();
}

void World::RemoveEntity(Entity* e)
{
	e->DropAllChildren();
	e->SetParent(Inside, nullptr,0,false,false);
	entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
	delete e;
}
void World::Tick()
{
	for (auto & entitie : entities) {
		entitie->Tick();
	}
	worldTime+=constants.daySpeed;
	if (worldTime > 75.f) {
		if (sunSet == false) {
			sunSet = true;
			ObservationManager::Instance().MakeObservation( new Observation_Direct("the sun sets",nullptr));
		}
	}
	if (worldTime > 100.f) {
		sunSet = false;
		ObservationManager::Instance().MakeObservation(new Observation_Direct("the sun rises", nullptr));
		worldTime = 0;
		day++;
	}
}




Entity* World::GetEntityByID(int id, int worldID)
{
	//If this tile is unloaded, load it now. Likely used due to an entity moving

	std::vector<int> loadedTiles = GameLoader::Instance().loadedTiles;
	if (std::find(loadedTiles.begin(), loadedTiles.end(), worldID) == loadedTiles.end() && worldID!=-1) {
		GameLoader::Instance().LoadTile(worldID);
		SetupParents();
	}
	for (auto & entitie : entities) {
		if (entitie->uniqueEntityID == id && entitie->worldID == worldID) {
			return entitie;
		}
	}
	return nullptr;
}

void World::MoveToTile(int tileName)
{
	//Save
	GameLoader::Instance().SavePlayer();


	GameLoader::Instance().UnloadTiles();
	GameLoader::Instance().loadedTiles.push_back(tileName);
	//Load
	GameLoader::Instance().LoadPlayer(false);
	GameLoader::Instance().currentPlayerTile = tileName;
	GameLoader::Instance().LoadTiles();
	SetupParents();
	playerEntity->SetParentOverride(On, GameLoader::Instance().currentGroundTile);
}



void World::SetupParents()
{
	//Set parents
	for (auto & entitie : entities) {
		if (entitie->worldID != -1) {
			entitie->SetParentOverride((Position)entitie->parentEntityDir, GetEntityByID(entitie->parentEntityID, entitie->worldID));
		}
		else {
			if (entitie == playerEntity) {
				entitie->SetParentOverride((Position)entitie->parentEntityDir, GetEntityByID(entitie->parentEntityID, GameLoader::Instance().currentPlayerTile));
			}
			else {
				if (entitie->parentEntityID == playerEntity->uniqueEntityID) {
					entitie->SetParentOverride((Position)entitie->parentEntityDir, playerEntity);
				}
				else {
					entitie->SetParentOverride((Position)entitie->parentEntityDir, GetEntityByID(entitie->parentEntityID, -1));
				}
			}
		}
	}
}

void World::SetupParents(int index)
{
	if (entities[index]->worldID != -1) {
		entities[index]->SetParentOverride((Position)entities[index]->parentEntityDir, GetEntityByID(entities[index]->parentEntityID, entities[index]->worldID));
	}
	else {
		if (entities[index]->parentEntityID == playerEntity->uniqueEntityID) {
			entities[index]->SetParentOverride((Position)entities[index]->parentEntityDir, playerEntity);
		}
		else {
			entities[index]->SetParentOverride((Position)entities[index]->parentEntityDir, GetEntityByID(entities[index]->parentEntityID, -1));
		}
	}
}

std::vector< Entity*> World::GetEntities()
{
	return entities;
}

float World::GetWorldTime()
{
	return worldTime;
}

int World::GetWorldDay()
{
	return day;
}

void World::SetWorldTime(float to)
{
	worldTime = to;
}

void World::SetWorldDay(int to)
{
	day = to;
}
