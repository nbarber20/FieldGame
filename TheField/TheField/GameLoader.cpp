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
#include "BehaviorTree.h"
#include "BehaviorNode.h"
#include "ObservationManager.h"


void GameLoader::Setup()
{
	currentFilename = "Testing";
	std::filesystem::create_directory("Data/Saves/" + currentFilename);
	int worldID = 0;

	Entity_GroundTile* Ground = new Entity_GroundTile();
	Ground->SetEntityData(GetUniqueID(), false, 0.0f, 60000.0f, 0.0f);
	Ground->attachedToParent = true;
	Ground->names = { "dirt","ground","grass" };
	loadedTiles = { worldID };
	Ground->worldPos = worldID;
	Ground->toEast = std::make_pair("There is a empty field to the east", 0);
	World::Instance().AddEntity(Ground);

	World::Instance().playerEntity = new Entity_Player();
	World::Instance().playerEntity->SetEntityData(GetUniqueID(), false, 0.0f, 3783.0f, 150.0f);
	World::Instance().playerEntity->names = { "you" };
	World::Instance().playerEntity->SetParent(On, Ground);
	World::Instance().AddEntity(World::Instance().playerEntity);


	BehaviorTree* RootHumanBehavior = new BehaviorTree("RootHumanBehavior", true);
	BehaviorNode* RootHumanBehaviorstart = new BehaviorNode();
	RootHumanBehavior->AddNode(RootHumanBehaviorstart, nullptr);

	BehaviorNode_Sequence* RootHumanBehaviorseq = new BehaviorNode_Sequence();
	RootHumanBehavior->AddNode(RootHumanBehaviorseq, RootHumanBehaviorstart);

	BehaviorNode_Living_AttackTarget* RootHumanBehaviorattack = new BehaviorNode_Living_AttackTarget(true);
	RootHumanBehavior->AddNode(RootHumanBehaviorattack, RootHumanBehaviorseq);


	SaveBehaviorTree(RootHumanBehavior);


	BehaviorTree* droneScanPlayer = new BehaviorTree("DroneScanPlayer", false);
	BehaviorNode* start = new BehaviorNode();
	droneScanPlayer->AddNode(start, nullptr);

	BehaviorNode_Sequence* seq = new BehaviorNode_Sequence();
	droneScanPlayer->AddNode(seq, start);

	BehaviorNode_WaitTicks* wait = new BehaviorNode_WaitTicks(1);
	droneScanPlayer->AddNode(wait, seq);


	BehaviorNode_AddObservation* observer = new BehaviorNode_AddObservation("GoGOGadget droneScan");
	droneScanPlayer->AddNode(observer, seq);

	SaveBehaviorTree(droneScanPlayer);



	Entity_Npc* NPC = new Entity_Npc();
	NPC->SetEntityData(GetUniqueID(), false, 0.0f, 3783.0f, 150.0f);
	NPC->names = { "person" };
	NPC->SetParent(On,Ground);

	//Make Tree
	DialogTree* dialogtree = new DialogTree();
	DialogTree::DialogNode node1;
	node1.dialog = "What Business do you have?";
	node1.responses.push_back(std::make_pair("Trading", 1));
	DialogTree::DialogNode node2;
	node2.dialog = "Drone active.";
	node2.behaviorTree = droneScanPlayer->treeName;
	dialogtree->TreeNodes.push_back(node1);
	dialogtree->TreeNodes.push_back(node2);

	//Finish npc setup
	NPC->dialogTree = dialogtree;
	dialogtree->LivingSource = NPC;
	NPC->AddBehavior(RootHumanBehavior);







	World::Instance().AddEntity(NPC);


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
			int hash = tree->nodes[i]->GetClassHash();
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

BehaviorNode* GameLoader::GenBehaviorNode(int hash)
{
	if (hash == typeid(BehaviorNode_Sequence*).hash_code()) {
		return new BehaviorNode_Sequence();
	}
	else if (hash == typeid(BehaviorNode_WaitTicks*).hash_code()) {
		return new BehaviorNode_WaitTicks();
	}
	else if (hash == typeid(BehaviorNode_AddObservation*).hash_code()) {
		return new BehaviorNode_AddObservation();
	}
	else if (hash == typeid(BehaviorNode_RunSubTree*).hash_code()) {
		return new BehaviorNode_RunSubTree();//might need special case
	}
	return new BehaviorNode();
	
		
}
