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
#include <iosfwd>
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
}


void World::Setup()
{
	
	Entity_GroundTile* Ground = new Entity_GroundTile(GetUniqueID(),false, 0.0f, 60000.0f);
	Ground->names = { "dirt","ground","grass" };
	currentTile = 1;
	Ground->worldPos = 1;
	Ground->toSouth = std::make_pair("There is a empty field to the south", 0);
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
	EnterFarmEvent->EventImageFile = "Data/Farmhouse.png";
	EnterFarmEvent->EventText = "You happen upon a farmhouse";
	EnterFarmEvent->SetParent(On, Ground, 0, true, false);
	entities.push_back(EnterFarmEvent);

	Entity_Interior* House = new Entity_Interior(GetUniqueID(),  false, 47520.0f, 47520.0f);
	House->names = { "house","home"};
	House->SetParent(On, Ground, 0, true, false);
	House->AddAdjective(Visual, "brick");
	House->AddAdjective(Inside, "cozy");
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
	SaveTile("Data/LevelData/" + std::to_string(currentTile) + ".bin", currentTile);
	for (int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	entities.clear();
	
	//Load
	LoadFile("Data/LevelData/Player.bin",true);
	currentTile = tileName;
	LoadFile("Data/LevelData/" + std::to_string(currentTile) + ".bin",false);
	playerEntity->SetParentOverride(On, currentGroundTile);
}

void World::SaveAll()
{
	SavePlayer();
	SaveTile("Data/LevelData/" + std::to_string(currentTile) + ".bin", currentTile);
}

void World::LoadAll()
{
	LoadFile("Data/LevelData/Player.bin", true);
	LoadFile("Data/LevelData/"+ std::to_string(currentTile) +".bin",false);
	playerEntity->SetParentOverride(On, currentGroundTile);
}

void World::SavePlayer()
{
	std::fstream file("Data/LevelData/Player.bin", std::ios::out | std::ios::binary);
	if (!file) {
		std::cout << "ERROR";
	}
	else {
		file.clear();

		file.write((char*) & (currentTile), sizeof(int));

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
				entities[i]->worldID = -1;
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

void World::SaveTile(std::string filename, int worldID) {
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file) {
		std::cout << "ERROR";
	}
	else {
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
					entities[i]->worldID = worldID;
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

void World::LoadFile(std::string filename, bool loadCurrentTile)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file) {
		std::cout << "ERROR";
	}
	else {
		if (loadCurrentTile) {
			file.read((char*)& currentTile, sizeof(int));
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

			Entity* e;
			if (entityObjType == "Entity_Clip") {
				e = new Entity_Clip();
			}
			else if (entityObjType == "Entity_Constructed") {
				e = new Entity_Constructed();
			}
			else if (entityObjType == "Entity_Container") {
				e = new Entity_Container();
			}
			else if (entityObjType == "Entity_Event") {
				e = new Entity_Event();
			}
			else if (entityObjType == "Entity_Firearm") {
				e = new Entity_Firearm();
			}
			else if (entityObjType == "Entity_Fluid") {
				e = new Entity_Fluid();
			}
			else if (entityObjType == "Entity_Food") {
				e = new Entity_Food();
			}
			else if (entityObjType == "Entity_GroundTile") {
				Entity_GroundTile* eg = new Entity_GroundTile();
				currentGroundTile = eg;
				e = eg;
			}
			else if (entityObjType == "Entity_Interior") {
				e = new Entity_Interior();
			}
			else if (entityObjType == "Entity_Living") {
				e = new Entity_Living();
			}
			else if (entityObjType == "Entity_Mechanisim") {
				e = new Entity_Mechanisim();
			}
			else if (entityObjType == "Entity_Npc") {
				e = new Entity_Npc();
			}
			else if (entityObjType == "Entity_Player") {
				Entity_Player* ep = new Entity_Player();
				playerEntity = ep;
				e = ep;
			}
			else if (entityObjType == "Entity_Readable") {
				e = new Entity_Readable();
			}
			else if (entityObjType == "Entity_Room") {
				e = new Entity_Room();
			}
			else {
				if (entityObjType != "Entity") {
					ObservationManager::Observation o = ObservationManager::Observation();
					o.sense = ObservationManager::SENSE_Look;
					o.type = ObservationManager::TYPE_Direct;
					o.information = "ERROR: "+ entityObjType+" not defined";
					ObservationManager::Instance().MakeObservation(o);
				}
				e = new Entity();
			}
			e->ReadData(&file);
			entities.push_back(e);
		}

		//Set parents
		for (int i = 0; i < entities.size(); i++) {
			Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
			if (p == NULL) {
				if (entities[i]->parentEntityID != -1) {
					Entity_Room* room = dynamic_cast<Entity_Room*>(entities[i]);
					if (room) {
						entities[i]->SetParent((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, entities[i]->worldID));
					}
					else {
						entities[i]->SetParentOverride((Position)entities[i]->parentEntityDir, GetEntityByID(entities[i]->parentEntityID, entities[i]->worldID));
					}
				}
			}
		}



		file.close();
	}
}