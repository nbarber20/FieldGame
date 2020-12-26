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
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Tick();
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
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->uniqueEntityID == id && entities[i]->worldID == worldID) {
			return entities[i];
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
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->worldID != -1) {
			entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, entities[i]->worldID));
		}
		else {
			if (entities[i] == playerEntity) {
				entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, GameLoader::Instance().currentPlayerTile));
			}
			else {
				if (entities[i]->parentEntityID == playerEntity->uniqueEntityID) {
					entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, playerEntity);
				}
				else {
					entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, -1));
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
