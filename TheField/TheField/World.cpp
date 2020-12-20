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
	int worldID = 1;

	Entity_GroundTile* Ground = new Entity_GroundTile(GetUniqueID(),false, 0.0f, 60000.0f);
	Ground->names = { "dirt","ground","grass" };
	loadedTiles = { worldID };
	Ground->worldPos = worldID;
	Ground->toNorth = std::make_pair("There is a empty field to the south", 0);
	entities.push_back(Ground);
	
	playerEntity = new Entity_Player(GetUniqueID(),  false, 0.0f, 3783.0f);
	playerEntity->names = { "you" };
	playerEntity->SetParent(On, nullptr);
	entities.push_back(playerEntity);

	Entity_Container* backPack = new Entity_Container(GetUniqueID(),  true, 1220.47f, 1221.0f);
	backPack->names = { "backpack" };
	backPack->SetParent(Back, playerEntity);
	backPack->AddAdjective(Visual, "leather");
	backPack->permiability = 4.5f;
	entities.push_back(backPack);

	Entity_Firearm* handGun = new Entity_Firearm(GetUniqueID(),  14.5f,Entity_Clip::Pistol);
	handGun->names = { "handgun", "gun" };
	handGun->SetParent(RightHand, playerEntity);
	entities.push_back(handGun);

	Entity_Clip* handGunAmmo = new Entity_Clip(GetUniqueID(),  7.f,7, Entity_Clip::Pistol);
	handGunAmmo->names = { "clip"};
	entities.push_back(handGunAmmo);

	handGun->Reload(handGunAmmo);

	Entity_Mechanisim* club = new Entity_Mechanisim(GetUniqueID(),  true, 0.0f, 14.5f);
	club->names = { "club" };
	std::vector<Task*> tasks3 = {
		new Task_LogText(club,"You swing the club"),
		new Task_AttackEntity(club,Entity_Living::Blunt,0.4f,2),
	};
	club->AddBehavior(std::make_pair("swing", tasks3));
	club->SetParent(LeftHand, playerEntity);
	entities.push_back(club);

	
	
	Entity_Event* EnterFarmEvent = new Entity_Event(GetUniqueID());
	EnterFarmEvent->setObservationConsumptionList({
		std::make_pair(ObservationManager::TYPE_All,ObservationManager::SENSE_All),
		});
	EnterFarmEvent->EventImageFile = "Data/Art/Farmhouse.png";
	EnterFarmEvent->EventText = "You happen upon a farmhouse";
	EnterFarmEvent->SetParent(On, Ground, 0, true, false);
	entities.push_back(EnterFarmEvent);

	Entity_Interior* House = new Entity_Interior(GetUniqueID(),  false, 47520.0f, 47520.0f);
	House->names = { "house","home"};
	House->SetParent(On, Ground, 0, true, false);
	House->AddAdjective(Visual, "brick");
	House->AddAdjective(Inside, "cozy");
	House->worldID = worldID;
	House->AddRoom("living room", sf::Vector2i(0, 0), true);
	House->AddRoom("bedroom", sf::Vector2i(0, 1), true);
	House->AddRoom("bathroom", sf::Vector2i(1, 0), true);
	entities.push_back(House);


	Entity_Npc* NPC = new Entity_Npc(GetUniqueID(),  false, 0.0f, 3783.0f);
	NPC->names = {"person"};
	NPC->SetParent(OnFloor, House, 0, false,false);

	//Make Tree
	DialogTree* tree = new DialogTree();
	DialogTree::DialogNode node1;
	node1.dialog = "The man greets you kindly, welcoming you to his humble farmhouse";
	node1.responses.push_back(std::make_pair("Greet the man", 1)); 
	node1.responses.push_back(std::make_pair("Insult the man", 2));
	DialogTree::DialogNode node2; 
	node2.dialog = "The man offers you food and supplies, whats his is yours";
	DialogTree::DialogNode node3;
	node3.dialog = "You insult the man, The guy makes a frowny face :C";
	tree->TreeNodes.push_back(node1);
	tree->TreeNodes.push_back(node2);
	tree->TreeNodes.push_back(node3);

	//Finish npc setup
	NPC->dialogTree = tree;
	entities.push_back(NPC);

	Entity_Event* EnterHouseEvent = new Entity_Event(GetUniqueID());
	EnterHouseEvent->setObservationConsumptionList({
		std::make_pair(ObservationManager::TYPE_All,ObservationManager::SENSE_All),
		});
	EnterHouseEvent->EventText = "Upon entering the farm house, a warm feeling rushes over you";
	EnterHouseEvent->SetParent(OnFloor, House, 0, false, false);
	entities.push_back(EnterHouseEvent);

	Entity* Toilet = new Entity_Container(GetUniqueID(),  true, 369.6f, 1728.0f);
	Toilet->names = { "toilet" };
	Toilet->SetParent(OnFloor, House, 2 ,true, false);
	entities.push_back(Toilet);

	Entity_Fluid* ToiletWater = new Entity_Fluid(GetUniqueID(),  true, 0.0f, 200.0f);
	ToiletWater->names = { "water" };
	ToiletWater->hydration = 10.0f;
	ToiletWater->SetParent(Inside, Toilet);
	entities.push_back(ToiletWater);


	Entity* Bed = new Entity(GetUniqueID(),  true, 0.0f, 50.0f);
	Bed->names = { "bed" };
	Bed->SetParent(OnFloor, House, 1, false, false);
	entities.push_back(Bed);


	Entity* Table = new Entity(GetUniqueID(),  true, 0.0f, 1728.0f);
	Table->names = { "table" };
	Table->SetParent(OnFloor, House,0, false, false);
	entities.push_back(Table);



	Entity_Mechanisim* UselessButton = new Entity_Mechanisim(GetUniqueID(),  true, 0.0f, 1728.0f);
	UselessButton->names = { "button" };
	UselessButton->SetParent(On, Table);
	std::vector<Task*> UselessButtontasks = {
		new Task_LogText(UselessButton,"You press the button. It suddenly disappears"),
		new Task_DestroySelf(UselessButton),
	};
	UselessButton->AddBehavior(std::make_pair("press", UselessButtontasks));
	entities.push_back(UselessButton);

	Entity_Food* Potato = new Entity_Food(GetUniqueID(),  true, 14.44f, 14.50f);
	Potato->names = { "potato" };
	Potato->lookInfo = "It's a cooked potato, still warm";
	Potato->nutritionalValue = 35.0f;
	Potato->SetParent(On, Table);
	entities.push_back(Potato);


	Entity_Container* Cup = new Entity_Container(GetUniqueID(),  true, 14.44f, 14.50f);
	Cup->names = { "cup" };
	Cup->SetParent(On, Table);
	Cup->AddAdjective(Visual, "wooden");
	entities.push_back(Cup);


	Entity_Fluid* Tea = new Entity_Fluid(GetUniqueID(),  true, 0.0f, 7.0f);
	Tea->names = { "tea" };
	Tea->SetParent(Inside, Cup);
	Tea->AddAdjective(Taste, "bitter");
	Tea->AddAdjective(Visual, "black");
	Tea->hydration = 45.0f;
	entities.push_back(Tea);

	Entity_Fluid* Tea2 = new Entity_Fluid(GetUniqueID(),  true, 0.0f, 7.0f);
	Tea2->names = { "tea" };
	Tea2->SetParent(Inside, Cup);
	Tea2->AddAdjective(Taste, "bitter");
	Tea2->AddAdjective(Visual, "black");
	Tea2->hydration = 35.0f;
	entities.push_back(Tea2);

	Entity* Cup2 = new Entity_Container(GetUniqueID(),  true, 14.44f, 14.50f);
	Cup2->names = { "cup", "container" };
	Cup2->SetParent(On, Table);
	Cup2->AddAdjective(Visual, "ugly");
	Cup2->AddAdjective(NameExtension, "liquid");
	entities.push_back(Cup2);

	Entity* Cup3 = new Entity_Container(GetUniqueID(),  true, 14.44f, 14.50f);
	Cup3->names = { "cup" };
	Cup3->SetParent(OnFloor, House, 0, false, false);
	Cup3->AddAdjective(Visual, "glass");
	entities.push_back(Cup3);

	Entity_Fluid* Tea3 = new Entity_Fluid(GetUniqueID(),  true, 0.0f, 0.0f);
	Tea3->names = { "tea" };
	Tea3->SetParent(Inside, Cup3);
	Tea3->AddAdjective(Taste, "smooth");
	Tea3->AddAdjective(Visual, "green");
	Tea3->hydration = 35.0f;
	entities.push_back(Tea3);
	
	Entity_Readable* Note = new Entity_Readable(GetUniqueID(),  true, 0.0f, 0.0f);
	Note->requiredLanguage = English;
	Note->text = "This is a note";
	Note->names = { "note" };
	Note->SetParent(On, Table);
	entities.push_back(Note);
	
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
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p == NULL) {
				if (entities[i]->IsChildOf(playerEntity) == false) {
					numEntities++;
				}
			}
		}
		file.write((char*)&numEntities, sizeof(int));
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p == NULL) {
				if (entities[i]->IsChildOf(playerEntity) == false) {
					entities[i]->worldID = tileID;
					entities[i]->WriteData(&file);
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
