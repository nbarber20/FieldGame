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
#include "Entity_Anomaly.h"
#include "Entity_Firearm.h"
#include "Entity_Room.h"
#include "Entity_Dispenser.h"
#include "BehaviorTree.h"
#include "BehaviorNode.h"
#include "BehaviorNode_Living.h"
#include "ObservationManager.h"
#include "MechanismBehavior.h"

void GameLoader::Setup()
{
	BehaviorTree* RootAnimalBehavior = new BehaviorTree("RootAnimalBehavior", true);
	BehaviorNode* start = new BehaviorNode();
	RootAnimalBehavior->AddNode(start, nullptr);

	BehaviorNode_Selector* GetWaterOrFoodSelector = new BehaviorNode_Selector();
	RootAnimalBehavior->AddNode(GetWaterOrFoodSelector, start);

	//WaterGraze
	BehaviorNode_Sequence* GetWater = new BehaviorNode_Sequence();
	RootAnimalBehavior->AddNode(GetWater, GetWaterOrFoodSelector);

	BehaviorNode_Living_IfThirsty* WaterCheck = new BehaviorNode_Living_IfThirsty(100);
	RootAnimalBehavior->AddNode(WaterCheck, GetWater);

	BehaviorNode_Graze* WaterGraze = new BehaviorNode_Graze();
	RootAnimalBehavior->AddNode(WaterGraze, GetWater);

	//FoodGraze
	BehaviorNode_Sequence* GetFood = new BehaviorNode_Sequence();
	RootAnimalBehavior->AddNode(GetWater, GetWaterOrFoodSelector);

	BehaviorNode_Living_IfHungry* FoodCheck = new BehaviorNode_Living_IfHungry(100);
	RootAnimalBehavior->AddNode(FoodCheck, GetFood);

	BehaviorNode_Graze* FoodGraze = new BehaviorNode_Graze();
	RootAnimalBehavior->AddNode(FoodGraze, GetFood);


	SaveBehaviorTree(RootAnimalBehavior);
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
		World::Instance().SetupParents();
	}
	return true;
}

Entity* GameLoader::SpawnPrefab(std::string filename, Position p, Entity* parent)
{
	std::fstream file("Data/Prefabs/" + filename + ".bin", std::ios::in | std::ios::binary);
	if (!file) {
		ThrowFileError("Error loading prefab");
	}
	else {
		int hash;
		file.read((char*)&hash, sizeof(int));
		Entity* e = GenEntity(hash);
		e->ReadData(&file);
		int index = World::Instance().AddEntity(e);
		e->SetParentOverride(p, parent);
		file.close();
		return e;
	}
	return nullptr;
}

void GameLoader::SavePrefab(Entity* e, std::string filename)
{
	std::fstream file("Data/Prefabs/" + filename + ".bin", std::ios::out | std::ios::binary);
	if (!file) {
		ThrowFileError("Error saving prefab");
	}
	else {
		file.clear();
		int hash = e->serializationID;
		file.write((char*)&hash, sizeof(int));
		e->WriteData(&file);
		file.close();
	}
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

BehaviorTree* GameLoader::LoadBehaviorTree(std::string filename)
{
	BehaviorTree* tree = new BehaviorTree(filename,false);
	std::fstream file("Data/BehaviorData/" + filename + ".bin", std::ios::in | std::ios::binary);
	if (!file) {
		ThrowFileError("Error loading BehaviorTree");
	}
	else {

		bool doLoop;
		file.read((char*)&doLoop, sizeof(bool));
		tree->loop = doLoop;
		int nodeCount;
		file.read((char*)&nodeCount, sizeof(int));
		for (int i = 0; i < nodeCount; i++) {
			int hash =0;
			file.read((char*)&hash, sizeof(int));
			BehaviorNode* newNode = GenBehaviorNode(hash);
			newNode->ReadData(&file);
			int parentIndex;
			file.read((char*)&parentIndex, sizeof(int));
			if (parentIndex >= 0) {
				tree->AddNode(newNode, tree->nodes[parentIndex]);
			}
			else {
				tree->AddNode(newNode, nullptr);
			}
		}
	}
	return tree;
}

void GameLoader::SaveBehaviorTree(BehaviorTree* tree)
{
	std::fstream file("Data/BehaviorData/" + tree->treeName + ".bin", std::ios::out | std::ios::binary);
	if (!file) {
		ThrowFileError("Error saving BehaviorTree");
	}
	else {
		bool doLoop = tree->loop;
		file.write((char*)&doLoop, sizeof(bool));
		int nodeCount = tree->nodes.size();
		file.write((char*)&nodeCount, sizeof(int));
		for (int i = 0; i < nodeCount; i++) {
			int hash = tree->nodes[i]->SerializationID;
			file.write((char*)&hash, sizeof(int));
			tree->nodes[i]->WriteData(&file);
			auto it = std::find(tree->nodes.begin(), tree->nodes.end(),tree->nodes[i]->nodeParent);
			if (it != tree->nodes.end())
			{
				int index = it - tree->nodes.begin();
				file.write((char*)&index, sizeof(int)); //writing parent node
			}
			else {
				int badIndex = -1;
				file.write((char*)&badIndex, sizeof(int)); //writing null parent node
			}
		}
		file.close();
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
				entities[i]->worldID = -1;
				int hash = entities[i]->serializationID;
				file.write((char*)&hash,sizeof(int));
				entities[i]->WriteData(&file);
			}
			else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
				entities[i]->worldID = -1;
				int hash = entities[i]->serializationID;
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
			e->worldID = -1;
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
				int hash = entities[i]->serializationID;
				file.write((char*)&hash, sizeof(int));
				entities[i]->WriteData(&file);
			}
			else
			{
				Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
				if (p == NULL) {
					if (entities[i]->IsChildOf(World::Instance().playerEntity) == false) {
						entities[i]->worldID = tileID;
						int hash = entities[i]->serializationID;
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
			try {
				e->ReadData(&file);
			}
			catch (...) {
				file.close();
				ThrowFileError("Error loading entity");
				return;
			}

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
	World::Instance().SetupParents();
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
		ThrowFileError("Important Game Data is Missing!(" + from + ")");
	}
}

void GameLoader::ThrowFileError(std::string error)
{
	//TODO asses
	errorCount = 0;
	MessageBoxA(NULL, error.c_str(), "ERROR", MB_OK);
	exit(EXIT_FAILURE);
}

Entity* GameLoader::GenEntity(int hash)
{
	switch (hash)
	{
		case 0:
			return new Entity();
		case 1:
			return new Entity_Clip(0,0,Entity_Clip::Pistol);
		case 2:
			return new Entity_Constructed(0,false);
		case 3:
			return new Entity_Container(0.f,0,false);
		case 4:
			return new Entity_Event("","");
		case 5:
			return new Entity_Firearm(Entity_Clip::Pistol,Entity_Living::Piercing,0,0);
		case 6:
			return new Entity_Fluid(0,false);
		case 7:
			return new Entity_Food(0,false);
		case 8:
			return new Entity_GroundTile();
		case 9 :
			return new Entity_Interior();
		case 10:
			return new Entity_Living();
		case 11:
			return new Entity_Mechanisim();
		case 12:
			return new Entity_Npc();
		case 13:
		{
			Entity_Player* ep = new Entity_Player();
			World::Instance().playerEntity = ep;
			return ep;
		}
			break;
		case 14:
			return new Entity_Readable(English,"",0,false);
		case 15:
			return new Entity_Room();
		case 16:
			return new Entity_Anomaly();
		case 17:
			return new Entity_Dispenser(0,"");
		default:
			//UNDEFINED
			return new Entity();
	}
}

BehaviorNode* GameLoader::GenBehaviorNode(int hash)
{
	switch (hash)
	{
	case 0:
		return new BehaviorNode();
	case 1:
		return new BehaviorNode_Sequence();
	case 2:
		return new BehaviorNode_Selector();
	case 3:
		return new BehaviorNode_ParallelSequence();
	case 4:
		return new BehaviorNode_WaitTicks(0);
	case 5:
		return new BehaviorNode_AddObservation("");
	case 6:
		return new BehaviorNode_WaitForObservation("");
	case 7:
		return new BehaviorNode_TargetEntityTypeInTarget(0);
	case 8:
		return new BehaviorNode_ActivateMechanism("");
	case 9:
		return new BehaviorNode_MoveToTarget();
	case 10:
		return new BehaviorNode_RunSubTree(nullptr);
	case 11:
		return new BehaviorNode_SpawnPrefab("",Nowhere,false);
	case 12:
		return new BehaviorNode_Living_SetTargetByName("");
	case 13:
		return new BehaviorNode_Living_GetSavedTarget("");
	case 14:
		return new BehaviorNode_Living_SetSubTargetByName("");
	case 15:
		return new BehaviorNode_Living_SetBehaviorState(Entity_Living::Idle);
	case 16:
		return new BehaviorNode_Living_AttackTarget(false);
	case 17:
		return new BehaviorNode_Living_DrinkTarget();
	case 18:
		return new BehaviorNode_Living_TargetEntityTypeInSelf(0);
	case 19:
		return new BehaviorNode_Living_IfThirsty(0);
	case 20:
		return new BehaviorNode_Living_ReturnHome();
	case 21:
		return new BehaviorNode_TargetDispenserType(0);
	case 22:
		return new BehaviorNode_UseDispenserAndTargetDispensed();
	case 23:
		return new BehaviorNode_Living_IfHungry(0);
	case 24:
		return new BehaviorNode_Living_EatTarget();
	case 25:
		return new BehaviorNode_Graze();
	default:
		//UNDEFINED
		return new BehaviorNode();
	}		
}
