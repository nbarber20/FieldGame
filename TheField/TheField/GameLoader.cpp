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
#include "ObservationManager.h"


void GameLoader::Setup()
{
	currentFilename = "Development";
	std::filesystem::create_directory("Data/Saves/" + currentFilename);
	int worldID = 3;

	Entity_GroundTile* Ground = new Entity_GroundTile();
	Ground->SetEntityData(GetUniqueID(), false, 0.0f, 60000.0f, 0.0f);
	Ground->attachedToParent = true;
	Ground->names = { "dirt","ground","grass" };
	loadedTiles = { worldID };
	Ground->worldPos = worldID;
	Ground->toEast = std::make_pair("There is a empty field to the east", 0);
	World::Instance().AddEntity(Ground);

	Entity_Event* EnterFarmEvent = new Entity_Event();
	EnterFarmEvent->SetEntityData(GetUniqueID(), false, 0.0f, 0.0f, 0.0f);
	EnterFarmEvent->setObservationConsumptionList({
		std::make_pair(ObservationManager::TYPE_All,ObservationManager::SENSE_All),
		});
	EnterFarmEvent->EventImageFile = "Data/Art/Horses.png";
	EnterFarmEvent->EventText = "There are two people in swat-like armor mounted on horses. ";
	EnterFarmEvent->SetParent(On, Ground, 0, true, false);
	World::Instance().AddEntity(EnterFarmEvent);

	Entity_Living* Horse = new Entity_Living();
	Horse->SetEntityData(GetUniqueID(), false, 0.0f, 30511.9f, 1000.f);
	Horse->names = { "horse" };
	Horse->AddAdjective(Visual, "white");
	Horse->SetParent(On, Ground);
	World::Instance().AddEntity(Horse);

	Entity_Npc* NPC = new Entity_Npc();
	NPC->SetEntityData(GetUniqueID(), false, 0.0f, 3783.0f, 100.f);
	NPC->names = { "person" };
	NPC->AddAdjective(Visual, "masked");
	NPC->SetParent(On, Horse);

	DialogTree* tree = new DialogTree();
	DialogTree::DialogNode node1;
	node1.dialog = "The masked man shouts: \"anomaly research, stand back!\"";
	tree->TreeNodes.push_back(node1);

	//Finish npc setup
	NPC->dialogTree = tree;
	World::Instance().AddEntity(NPC);


	Entity_Living* Horse2 = new Entity_Living();
	Horse2->SetEntityData(GetUniqueID(), false, 0.0f, 30511.9f, 1000.f);
	Horse2->names = { "horse" };
	Horse2->AddAdjective(Visual, "brown");
	Horse2->SetParent(On, Ground);
	World::Instance().AddEntity(Horse2);

	Entity_Npc* NPC2 = new Entity_Npc();
	NPC2->SetEntityData(GetUniqueID(), false, 0.0f, 3783.0f, 100.f);
	NPC2->names = { "person" };
	NPC2->AddAdjective(Visual, "armed");
	NPC2->SetParent(On, Horse2);

	DialogTree* tree2 = new DialogTree();
	DialogTree::DialogNode node12;
	node12.dialog = "The armed man shouts: \"anomaly research, stand back!\"";
	tree2->TreeNodes.push_back(node12);

	//Finish npc setup
	NPC2->dialogTree = tree2;
	World::Instance().AddEntity(NPC2);

	Entity_Firearm* Ak47 = new Entity_Firearm(Entity_Clip::MachineGun);
	Ak47->SetEntityData(GetUniqueID(), false, 0.0, 1000.0f, 7.5f);
	Ak47->names = { "ak47", "gun" };
	Ak47->SetParent(Back, NPC);
	World::Instance().AddEntity(Ak47);

	Entity_Clip* Ak47Ammo = new Entity_Clip(7, Entity_Clip::MachineGun);
	Ak47Ammo->SetEntityData(GetUniqueID(), false, 0.0, 7.f, 0.5f);
	Ak47Ammo->names = { "clip" };
	World::Instance().AddEntity(Ak47Ammo);
	Ak47->Reload(Ak47Ammo);

	Entity_Firearm* M16 = new Entity_Firearm(Entity_Clip::MachineGun);
	M16->SetEntityData(GetUniqueID(), false, 0.0, 1000.0f, 7.5f);
	M16->names = { "m16", "gun" };
	M16->SetParent(RightHand, NPC2);
	World::Instance().AddEntity(M16);

	Entity_Clip* M16Ammo = new Entity_Clip(7, Entity_Clip::MachineGun);
	Ak47Ammo->SetEntityData(GetUniqueID(), false, 0.0, 7.f, 0.5f);
	M16Ammo->names = { "clip" };
	World::Instance().AddEntity(M16Ammo);
	M16->Reload(M16Ammo);
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
				int hash = entities[i]->GetClassHash();
				file.write((char*)&hash,sizeof(int));
				entities[i]->WriteData(&file);
			}
			else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
				entities[i]->worldID = -1;
				int hash = entities[i]->GetClassHash();
				file.write((char*)&hash, sizeof(int));
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
			int hash;
			file.read((char*)&hash, sizeof(int));
			Entity* e = GenEntity(hash);
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
				int hash = entities[i]->GetClassHash();
				file.write((char*)&hash, sizeof(int));
				entities[i]->WriteData(&file);
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(World::Instance().playerEntity) == false) {
						entities[i]->worldID = tileID;
						int hash = entities[i]->GetClassHash();
						file.write((char*)&hash, sizeof(int));
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

			int hash;
			file.read((char*)&hash, sizeof(int));
			Entity* e = GenEntity(hash);
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

Entity* GameLoader::GenEntity(int hash)
{
	if (hash == typeid(Entity_Clip*).hash_code()) {
		return new Entity_Clip();
	}
	else if (hash == typeid(Entity_Constructed*).hash_code()) {
		return new Entity_Constructed();
	}
	else if (hash == typeid(Entity_Container*).hash_code()) {
		return new Entity_Container();
	}
	else if (hash == typeid(Entity_Event*).hash_code()) {
		return new Entity_Event();
	}
	else if (hash == typeid(Entity_Firearm*).hash_code()) {
		return new Entity_Firearm(Entity_Clip::Pistol);
	}
	else if (hash == typeid(Entity_Fluid*).hash_code()) {
		return new Entity_Fluid();
	}
	else if (hash == typeid(Entity_Food*).hash_code()) {
		return new Entity_Food();
	}
	else if (hash == typeid(Entity_GroundTile*).hash_code()) {
		return new Entity_GroundTile();
	}
	else if (hash == typeid(Entity_Interior*).hash_code()) {
		return new Entity_Interior();
	}
	else if (hash == typeid(Entity_Living*).hash_code()) {
		return new Entity_Living();
	}
	else if (hash == typeid(Entity_Mechanisim*).hash_code()) {
		return new Entity_Mechanisim();
	}
	else if (hash == typeid(Entity_Npc*).hash_code()) {
		return new Entity_Npc();
	}
	else if (hash == typeid(Entity_Player*).hash_code()) {
		Entity_Player* ep = new Entity_Player();
		World::Instance().playerEntity = ep;
		return ep;
	}
	else if (hash == typeid(Entity_Readable*).hash_code()) {
		return new Entity_Readable();
	}
	else if (hash == typeid(Entity_Room*).hash_code()) {
		return new Entity_Room();
	}

	else if (hash != typeid(Entity*).hash_code()) {
		//Undefined hash
	}
	return new Entity();
}