#include "pch.h"
#include "GameLoader.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iosfwd>
#include <windows.h>
#include "World.h"
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

void GameLoader::Setup()
{

}


int GameLoader::GetUniqueID()
{
	return uniqueID++;
}


void GameLoader::SaveAll()
{
	SavePlayer();
	for (int i = 0; i < loadedTiles.size(); i++) {
		SaveTile(loadedTiles[i]);
	}
}

bool GameLoader::LoadAll(std::string filename)
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
		World::Instance().setupParents();
	}
	return true;
}

void GameLoader::UnloadTiles()
{
	for (int i = 0; i < loadedTiles.size(); i++) {
		SaveTile(loadedTiles[i]);
	}


	std::vector<int> newTiles;
	std::vector<Entity*> entities = World::Instance().GetEntities();
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
	World::Instance().ClearEntities();
	loadedTiles = newTiles;
}

void GameLoader::LoadTiles()
{
	for (int i = 0; i < loadedTiles.size(); i++) {
		LoadTile(loadedTiles[i]);
	}
}

void GameLoader::SavePlayer()
{
	std::fstream file("Data/Saves/" + currentFilename + "/Player.bin", std::ios::out | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error saving player");
		}
		else {
			CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/" + currentFilename + "/Player.bin");
			SavePlayer();
			return;
		}
	}
	else {
		errorCount = 0;
		file.clear();

		int day = World::Instance().GetWorldDay();
		file.write((char*)&day, sizeof(int));
		int time = World::Instance().GetWorldTime();
		file.write((char*)&time, sizeof(float));
		file.write((char*)&currentPlayerTile, sizeof(int));
		int loadedTilesLen = loadedTiles.size();
		file.write((char*)&loadedTilesLen, sizeof(int));
		for (int j = 0; j < loadedTilesLen; j++) {
			int loaded = loadedTiles[j];
			file.write((char*)&loaded, sizeof(int));
		}

		std::vector<Entity*> entities = World::Instance().GetEntities();
		int numEntities = 0;
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p) {
				numEntities++;
			}
			else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
				numEntities++;
			}
		}

		file.write((char*)&numEntities, sizeof(int));
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p) {
				entities[i]->worldID = currentPlayerTile;
				entities[i]->WriteData(&file);
			}
			else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
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

void GameLoader::LoadPlayer(bool getLoadedTiles)
{
	std::fstream file("Data/Saves/" + currentFilename + "/Player.bin", std::ios::in | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error loading player");
		}
		else {
			CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/" + currentFilename + "/Player.bin");
			LoadPlayer(getLoadedTiles);
			return;
		}
	}
	else {
		errorCount = 0;
		//Get loadedTiles from player

		int day;
		file.read((char*)&day, sizeof(int));
		World::Instance().SetWorldDay(day);
		float worldTime;
		file.read((char*)&worldTime, sizeof(float));
		World::Instance().SetWorldTime(worldTime);
		if (getLoadedTiles) {

			file.read((char*)&currentPlayerTile, sizeof(int));
			loadedTiles.clear();
			int loadedTilesLen;
			file.read((char*)&loadedTilesLen, sizeof(int));
			for (int j = 0; j < loadedTilesLen; j++) {
				int loaded;
				file.read((char*)&loaded, sizeof(int));
				loadedTiles.push_back(loaded);
			}
		}
		else {
			int t;
			file.read((char*)&t, sizeof(int));
			int len;
			file.read((char*)&len, sizeof(int));
			for (int j = 0; j < len; j++) {
				int loaded;
				file.read((char*)&loaded, sizeof(int));
			}
		}

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

			Entity* e = GenEntity(entityObjType);
			e->ReadData(&file);
			World::Instance().AddEntity(e);
		}
		file.close();
	}
}

void  GameLoader::SaveTile(int tileID) {
	std::fstream file("Data/Saves/" + currentFilename + "/" + std::to_string(tileID) + ".bin", std::ios::out | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error saving tile data");
		}
		else {
			CopyGameFile("Data/WorldData/" + std::to_string(tileID) + ".bin", "Data/Saves/" + currentFilename + "/" + std::to_string(tileID) + ".bin");
			SaveTile(tileID);
			return;
		}
	}
	else {
		errorCount = 0;
		file.clear();
		int numEntities = 0;
		std::vector<Entity*> entities = World::Instance().GetEntities();
		for (int i = 0; i < entities.size(); i++) {
			if (World::Instance().playerEntity == NULL) {
				numEntities++;
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(World::Instance().playerEntity) == false) {
						numEntities++;
					}
				}
			}
		}
		file.write((char*)&numEntities, sizeof(int));
		for (int i = 0; i < entities.size(); i++) {
			if (World::Instance().playerEntity == NULL) {
				entities[i]->worldID = tileID;
				entities[i]->WriteData(&file);
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(World::Instance().playerEntity) == false) {
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

void GameLoader::LoadTile(int tileID)
{
	if (tileID < 0)return;
	std::fstream file("Data/Saves/" + currentFilename + "/" + std::to_string(tileID) + ".bin", std::ios::in | std::ios::binary);
	if (!file) {
		errorCount++;
		if (errorCount > 3) {
			ThrowFileError("Error loading tile data");
		}
		else {
			CopyGameFile("Data/WorldData/" + std::to_string(tileID) + ".bin", "Data/Saves/" + currentFilename + "/" + std::to_string(tileID) + ".bin");
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

			Entity* e = GenEntity(entityObjType);
			e->ReadData(&file);
			//Get the ground tile to put the player on TODO: maybe a better way of finding where to parent player
			Entity_GroundTile* groundTile = dynamic_cast<Entity_GroundTile*>(e);
			if (groundTile) {
				if (groundTile->worldID == currentPlayerTile) {
					currentGroundTile = groundTile;
				}
			}
			World::Instance().AddEntity(e);
		}
		file.close();
	}
}

bool GameLoader::CreateNewGameFile(std::string filename)
{
	std::filesystem::create_directory("Data/Saves/");
	if (std::filesystem::create_directory("Data/Saves/" + filename) == false)
	{
		return false;
	}
	currentFilename = filename;
	CopyGameFile("Data/WorldData/Player.bin", "Data/Saves/" + currentFilename + "/Player.bin");
	LoadPlayer(true);
	LoadTile(0);
	World::Instance().setupParents();
	World::Instance().playerEntity->SetParentOverride(On, currentGroundTile);
	return true;
}

bool GameLoader::DeleteGameFile(std::string filename)
{
	std::filesystem::create_directory("Data/Saves/");
	if (std::filesystem::remove_all("Data/Saves/" + filename) == false)
	{
		return false;
	}
	return true;
}

void GameLoader::CopyGameFile(std::string from, std::string to)
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
		MessageBoxA(NULL, s.c_str(), "ERROR", MB_OK);
		exit(EXIT_FAILURE);
	}
}

void GameLoader::ThrowFileError(std::string error)
{
	errorCount = 0;
}

Entity* GameLoader::GenEntity(std::string entityObjType)
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
		World::Instance().playerEntity = ep;
		return ep;
	}
	else if (entityObjType == "Entity_Readable") {
		return new Entity_Readable();
	}
	else if (entityObjType == "Entity_Room") {
		return new Entity_Room();
	}

	if (entityObjType != "Entity") {
		//Undefined hash
	}
	return new Entity();
}