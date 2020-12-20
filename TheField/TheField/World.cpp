#include "pch.h"
#include "World.h"
#include "Entity.h"
#include "Entity_GroundTile.h"
#include "Entity_Player.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "Entity_Readable.h"
#include "Entity_Interior.h"
#include "Entity_Event.h"
#include "Entity_Food.h"
#include "Entity_Npc.h"
#include "Entity_Mechanisim.h"
#include "Entity_Clip.h"
#include "Entity_Firearm.h"
#include "Entity_Room.h"
#include "ObservationManager.h"
#include "Task.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iosfwd>
#include <windows.h>
World::World()
{
}

void World::AddEntity(Entity* e)
{
	entities.push_back(e);
}

void World::RemoveEntity(Entity* e)
{
	ObservationManager::Instance().RemoveObservationForEntity(e);
	e->DropAllChildren();
	e->SetParent(Inside, nullptr,0,false,false);
	entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
	delete e;
}

int World::GetUniqueID()
{
	return uniqueID++;
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
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.information = "The sun sets";
			ObservationManager::Instance().MakeObservation(o);
		}
	}
	if (worldTime > 100.f) {
		sunSet = false;
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "The sun rises";
		ObservationManager::Instance().MakeObservation(o);
		worldTime = 0;
		day++;
	}
}


void World::Setup()
{
	currentFilename = "Development";
	std::filesystem::create_directory("Data/Saves/" + currentFilename);
	int worldID = 0;

	Entity_GroundTile* Ground = new Entity_GroundTile(GetUniqueID(), false, 0.0f, 60000.0f);
	Ground->names = { "dirt","ground","grass" };
	loadedTiles = { worldID };
	Ground->worldPos = worldID;
	Ground->toWest = std::make_pair("There is a people on horses to the west", 3);
	Ground->toNorth = std::make_pair("There is a farmhouse to the north", 1);
	entities.push_back(Ground);
 
	playerEntity = new Entity_Player(GetUniqueID(), false, 0.0f, 3783.0f);
	playerEntity->names = { "you" };
	playerEntity->SetParent(On, Ground);
	entities.push_back(playerEntity);

	Entity_Container* backPack = new Entity_Container(GetUniqueID(), true, 1220.47f, 1221.0f);
	backPack->names = { "backpack" };
	backPack->SetParent(Back, playerEntity);
	backPack->AddAdjective(Visual, "leather");
	backPack->permiability = 4.5f;
	entities.push_back(backPack);

	Entity_Firearm* handGun = new Entity_Firearm(GetUniqueID(), 14.5f, Entity_Clip::Pistol);
	handGun->names = { "handgun", "gun" };
	handGun->SetParent(RightHand, playerEntity);
	entities.push_back(handGun);

	Entity_Clip* handGunAmmo = new Entity_Clip(GetUniqueID(), 7.f, 7, Entity_Clip::Pistol);
	handGunAmmo->names = { "clip" };
	entities.push_back(handGunAmmo);

	handGun->Reload(handGunAmmo);
}

Entity* World::GetEntityByID(int id, int worldID)
{
	//If this tile is unloaded, load it now. Likely used due to an entity moving
	if (std::find(loadedTiles.begin(), loadedTiles.end(), worldID) == loadedTiles.end()) {
		LoadTile(worldID);
		setupParents();
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
	SavePlayer();


	for (int i = 0; i < loadedTiles.size(); i++) {
		SaveTile(loadedTiles[i]);
	}
	std::vector<int> newTiles;
	for (int i = 0; i < entities.size(); i++) {
		Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
		if (p == NULL) {
			if (entities[i]->worldActive == true) {
				if (std::find(loadedTiles.begin(), loadedTiles.end(), entities[i]->worldID) == loadedTiles.end()) {
					newTiles.push_back(entities[i]->worldID);
				}
			}
		}
		delete entities[i];
	}
	loadedTiles = newTiles;
	entities.clear();
	
	loadedTiles.push_back(tileName);
	//Load
	LoadPlayer(false);
	playerEntity->worldID = tileName;

	currentPlayerTile = tileName;
	for (int i = 0; i < loadedTiles.size(); i++) {
		LoadTile(loadedTiles[i]);
	}
	setupParents();
	playerEntity->SetParentOverride(On, currentGroundTile);
}

void World::SaveAll()
{
	SavePlayer();
	for (int i = 0; i < loadedTiles.size(); i++) {
		SaveTile(loadedTiles[i]);
	}
}

bool World::LoadAll(std::string filename)
{
	currentFilename = filename;

	std::fstream file("Data/Saves/" + currentFilename + "/Player.bin", std::ios::in | std::ios::binary);
	if (!file) {
		return false;
	}
	else {
		LoadPlayer(true);
		for (int i = 0; i < loadedTiles.size(); i++) {
			LoadTile(loadedTiles[i]);
		}
		setupParents();
	}
	return true;
}

void World::SavePlayer()
{
	std::fstream file("Data/Saves/"+ currentFilename +"/Player.bin", std::ios::out | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error saving player");
		}
		else {
			CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/"+ currentFilename +"/Player.bin");
			SavePlayer();
			return;
		}
	}
	else {
		errorCount = 0;
		file.clear();

		file.write((char*)&day, sizeof(int));
		file.write((char*)&worldTime, sizeof(float));
		file.write((char*)& currentPlayerTile, sizeof(int));
		int loadedTilesLen = loadedTiles.size();
		file.write((char*)& loadedTilesLen, sizeof(int));
		for (int j = 0; j < loadedTilesLen; j++) {
			int loaded = loadedTiles[j];
			file.write((char*)& loaded, sizeof(int));
		}


		int numEntities = 0;
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p) {
				numEntities++;
			}
			else if (entities[i]->IsChildOf(playerEntity) == true) {
				numEntities++;
			}
		}

		file.write((char*)& numEntities, sizeof(int));
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p) {
				entities[i]->worldID = currentPlayerTile;
				entities[i]->WriteData(&file);
			}
			else if (entities[i]->IsChildOf(playerEntity) == true) {
				entities[i]->worldID = -1;
				entities[i]->WriteData(&file);
			}
		}
		file.flush();
		file.close();
		if (!file.good()) {
			std::cout << "Error occurred at writing time!" << std::endl;
		}
	}
}

void World::LoadPlayer(bool getLoadedTiles)
{
	std::fstream file("Data/Saves/"+ currentFilename +"/Player.bin", std::ios::in | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error loading player");
		}
		else {
			CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/"+ currentFilename +"/Player.bin");
			LoadPlayer(getLoadedTiles);
			return;
		}
	}
	else {
		errorCount = 0;
		//Get loadedTiles from player


		file.read((char*)&day, sizeof(int));
		file.read((char*)&worldTime, sizeof(float));
		if (getLoadedTiles) {

			file.read((char*)&currentPlayerTile, sizeof(int));
			loadedTiles.clear();
			int loadedTilesLen;
			file.read((char*)& loadedTilesLen, sizeof(int));
			for (int j = 0; j < loadedTilesLen; j++) {
				int loaded;
				file.read((char*)& loaded, sizeof(int));
				loadedTiles.push_back(loaded);
			}
		}
		else {
			int t;
			file.read((char*)& t, sizeof(int));
			int len;
			file.read((char*)& len, sizeof(int));
			for (int j = 0; j < len; j++) {
				int loaded;
				file.read((char*)& loaded, sizeof(int));
			}
		}

		int entitiesArrSize;
		file.read((char*)& entitiesArrSize, sizeof(int));
		for (int i = 0; i < entitiesArrSize; i++) {

			std::string entityObjType;
			size_t namelen;
			file.read((char*)& namelen, sizeof(size_t));
			char* temp = new char[namelen + 1];
			file.read(temp, namelen);
			temp[namelen] = '\0';
			entityObjType = temp;
			delete[] temp;

			Entity* e = genEntity(entityObjType);
			e->ReadData(&file);
			entities.push_back(e);
		}
		file.close();
	}
}

void  World::SaveTile(int tileID){
	std::fstream file("Data/Saves/"+ currentFilename +"/" + std::to_string(tileID) + ".bin", std::ios::out | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error saving tile data");
		}
		else {
			CopyGameFile("Data/WorldData/" + std::to_string(tileID) + ".bin", "Data/Saves/"+ currentFilename +"/" + std::to_string(tileID) + ".bin");
			SaveTile(tileID);
			return;
		}
	}
	else {
		errorCount = 0;
		file.clear();
		int numEntities = 0;
		for (int i = 0; i < entities.size(); i++) {
			if (playerEntity == NULL) {
				numEntities++;
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(playerEntity) == false) {
						numEntities++;
					}
				}
			}
		}
		file.write((char*)&numEntities, sizeof(int));
		for (int i = 0; i < entities.size(); i++) {
			if (playerEntity == NULL) {
				entities[i]->worldID = tileID;
				entities[i]->WriteData(&file);
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(playerEntity) == false) {
						entities[i]->worldID = tileID;
						entities[i]->WriteData(&file);
					}
				}
			}
		}
		file.flush();
		file.close();
		if (!file.good()) {
			std::cout << "Error occurred at writing time!" << std::endl;
		}
	}
}

void World::LoadTile(int tileID)
{
	if (tileID < 0)return;
	std::fstream file("Data/Saves/"+ currentFilename +"/" + std::to_string(tileID) + ".bin", std::ios::in | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error loading tile data");
		}
		else {
			CopyGameFile("Data/WorldData/" + std::to_string(tileID) + ".bin", "Data/Saves/"+ currentFilename +"/" + std::to_string(tileID) + ".bin");
			LoadTile(tileID);
			return;
		}
	}
	else {
		errorCount = 0;
		int entitiesArrSize;
		file.read((char*)&entitiesArrSize, sizeof(int));
		for (int i = 0; i < entitiesArrSize; i++) {

			std::string entityObjType;
			size_t namelen;
			file.read((char*)&namelen, sizeof(size_t));
			char* temp = new char[namelen + 1];
			file.read(temp, namelen);
			temp[namelen] = '\0';
			entityObjType = temp;
			delete[] temp;

			Entity* e = genEntity(entityObjType);
			e->ReadData(&file);
			//Get the ground tile to put the player on TODO: maybe a better way of finding where to parent player
			Entity_GroundTile* groundTile = dynamic_cast<Entity_GroundTile*>(e);
			if (groundTile) {
				if (groundTile->worldID == currentPlayerTile) {
					currentGroundTile = groundTile;
				}
			}			
			entities.push_back(e);
		}
		file.close();
	}
}

bool World::CreateNewGameFile(std::string filename)
{
	std::filesystem::create_directory("Data/Saves/");
	if (std::filesystem::create_directory("Data/Saves/" + filename) == false)
	{
		return false;
	}
	currentFilename = filename;
	CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/"+ currentFilename +"/Player.bin");
	LoadPlayer(true);
	LoadTile(0);
	setupParents();
	playerEntity->SetParentOverride(On, currentGroundTile);
	return true;
}

bool World::DeleteGameFile(std::string filename)
{
	std::filesystem::create_directory("Data/Saves/");
	if (std::filesystem::remove_all("Data/Saves/" + filename) == false)
	{
		return false;
	}
	return true;
}

void World::CopyGameFile(std::string from, std::string to)
{
	std::ifstream src;
	std::ofstream dst;

	src.open(from, std::ios::in | std::ios::binary);
	if (src) {
		dst.open(to, std::ios::out | std::ios::binary);
		dst << src.rdbuf();

		src.close();
		dst.close();
	}
	else {
		std::string s = "Important Game Data is Missing!(" + from + ")";
		MessageBoxA(NULL, s.c_str(), "ERROR" , MB_OK);
		exit(EXIT_FAILURE);
	}
}

void World::ThrowFileError(std::string error)
{
	errorCount = 0;
	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Direct;
	o.information = error;
	ObservationManager::Instance().MakeObservation(o);
}

Entity* World::genEntity(std::string entityObjType)
{
	if (entityObjType == "Entity_Clip") {
		return new Entity_Clip();
	}
	else if (entityObjType == "Entity_Constructed") {
		return new Entity_Constructed();
	}
	else if (entityObjType == "Entity_Container") {
		return new Entity_Container();
	}
	else if (entityObjType == "Entity_Event") {
		return new Entity_Event();
	}
	else if (entityObjType == "Entity_Firearm") {
		return new Entity_Firearm();
	}
	else if (entityObjType == "Entity_Fluid") {
		return new Entity_Fluid();
	}
	else if (entityObjType == "Entity_Food") {
		return new Entity_Food();
	}
	else if (entityObjType == "Entity_GroundTile") {
		return new Entity_GroundTile();
	}
	else if (entityObjType == "Entity_Interior") {
		return new Entity_Interior();
	}
	else if (entityObjType == "Entity_Living") {
		return new Entity_Living();
	}
	else if (entityObjType == "Entity_Mechanisim") {
		return new Entity_Mechanisim();
	}
	else if (entityObjType == "Entity_Npc") {
		return new Entity_Npc();
	}
	else if (entityObjType == "Entity_Player") {
		Entity_Player* ep = new Entity_Player();
		playerEntity = ep;
		return ep;
	}
	else if (entityObjType == "Entity_Readable") {
		return new Entity_Readable();
	}
	else if (entityObjType == "Entity_Room") {
		return new Entity_Room();
	}

	if (entityObjType != "Entity") {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "ERROR: " + entityObjType + " not defined";
		ObservationManager::Instance().MakeObservation(o);
	}
	return new Entity();
}

void World::setupParents()
{
	//Set parents
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->worldID != -1) {
			entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, entities[i]->worldID));
		}
		else {
			entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, playerEntity);
		}
	}
}
