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
#include "ObservationManager.h"
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

void World::Tick()
{
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Tick();
	}
}


void World::Setup()
{

	Entity* Ground = new Entity_GroundTile(false, 0.0f, 60000.0f);
	Ground->names = {"dirt","ground","grass"};
	entities.push_back(Ground);

	playerEntity = new Entity_Player(false, 0.0f, 3783.0f);
	playerEntity->names = { "you" };
	playerEntity->SetParent(On, Ground);
	entities.push_back(playerEntity);

	Entity_Container* backPack = new Entity_Container(true, 1220.47f, 1221.0f);
	backPack->names = { "backpack" };
	backPack->SetParent(Back, playerEntity);
	backPack->AddAdjective(Visual, "leather");
	backPack->permiability = 4.5f;
	entities.push_back(backPack);


	Entity_Interior* House = new Entity_Interior(false, 47520.0f, 47520.0f);
	House->names = { "house","home"};
	House->SetParent(On, Ground, 0, true, false);
	House->AddAdjective(Visual, "brick");
	House->AddAdjective(Inside, "cozy");
	House->AddRoom("living room", sf::Vector2i(0, 0), true);
	House->AddRoom("bedroom", sf::Vector2i(0, 1), true);
	House->AddRoom("bathroom", sf::Vector2i(1, 0), true);
	entities.push_back(House);


	Entity_Npc* NPC = new Entity_Npc(false, 0.0f, 3783.0f);
	NPC->names = {"person"};
	NPC->SetParent(OnFloor, House, 0, false,false);

	//Make Tree
	DialogTree* tree = new DialogTree();
	DialogTree::DialogNode node1;
	node1.dialog = "The man greets you kindly, welcoming you to his humble farmhouse.";
	node1.responses.push_back(std::make_pair("Greet the man", 1)); 
	node1.responses.push_back(std::make_pair("Insult the man", 2));
	DialogTree::DialogNode node2; 
	node2.dialog = "The man offers you food and supplies, whats his is yours.";
	DialogTree::DialogNode node3;
	node3.dialog = "You insult the man, The guy makes a frowny face :C";
	tree->TreeNodes.push_back(node1);
	tree->TreeNodes.push_back(node2);
	tree->TreeNodes.push_back(node3);

	//Finish npc setup
	NPC->dialogTree = tree;
	entities.push_back(NPC);

	Entity_Event* EnterHouseEvent = new Entity_Event();
	EnterHouseEvent->setObservationConsumptionList({
		std::make_pair(ObservationManager::TYPE_All,ObservationManager::SENSE_All),
		});
	EnterHouseEvent->EventText = "Upon entering the farm house, a warm feeling rushes over you.";
	EnterHouseEvent->SetParent(OnFloor, House, 0, false, false);
	entities.push_back(EnterHouseEvent);

	Entity* Toilet = new Entity_Container(true, 369.6f, 1728.0f);
	Toilet->names = { "toilet" };
	Toilet->SetParent(OnFloor, House, 2 ,true, false);
	entities.push_back(Toilet);

	Entity* ToiletWater = new Entity_Fluid(true, 0.0f, 200.0f);
	ToiletWater->names = { "water" };
	ToiletWater->SetParent(Inside, Toilet);
	entities.push_back(ToiletWater);


	Entity* Bed = new Entity(true, 0.0f, 50.0f);
	Bed->names = { "bed" };
	Bed->SetParent(OnFloor, House, 1, false, false);
	entities.push_back(Bed);



	Entity* Table = new Entity(true, 0.0f, 1728.0f);
	Table->names = { "table" };
	Table->SetParent(OnFloor, House,0, false, false);
	entities.push_back(Table);



	Entity_Mechanisim* UselessButton = new Entity_Mechanisim(true, 0.0f, 1728.0f);
	UselessButton->names = { "button" };
	UselessButton->SetParent(On, Table);
	std::vector<Task*> tasks = {
		new Task_LogText("You press the button. It suddenly disappears."),
		new Task_DestroySelf(UselessButton),
	};
	UselessButton->AddBehavior(std::make_pair("press", tasks));
	entities.push_back(UselessButton);

	Entity_Food* Potato = new Entity_Food(true, 14.44f, 14.50f);
	Potato->names = { "potato" };
	Potato->SetParent(On, Table);
	entities.push_back(Potato);


	Entity_Container* Cup = new Entity_Container(true, 14.44f, 14.50f);
	Cup->names = { "cup" };
	Cup->SetParent(On, Table);
	Cup->AddAdjective(Visual, "wooden");
	entities.push_back(Cup);


	Entity* Tea = new Entity_Fluid(true, 0.0f, 7.0f);
	Tea->names = { "tea" };
	Tea->SetParent(Inside, Cup);
	Tea->AddAdjective(Taste, "bitter");
	Tea->AddAdjective(Visual, "black");
	entities.push_back(Tea);

	Entity* Tea2 = new Entity_Fluid(true, 0.0f, 7.0f);
	Tea2->names = { "tea" };
	Tea2->SetParent(Inside, Cup);
	Tea2->AddAdjective(Taste, "bitter");
	Tea2->AddAdjective(Visual, "black");
	entities.push_back(Tea2);

	Entity* Cup2 = new Entity_Container(true, 14.44f, 14.50f);
	Cup2->names = { "cup", "container" };
	Cup2->SetParent(On, Table);
	Cup2->AddAdjective(Visual, "ugly");
	Cup2->AddAdjective(NameExtension, "liquid");
	entities.push_back(Cup2);

	Entity* Cup3 = new Entity_Container(true, 14.44f, 14.50f);
	Cup3->names = { "cup" };
	Cup3->SetParent(OnFloor, House, 0, false, false);
	Cup3->AddAdjective(Visual, "glass");
	entities.push_back(Cup3);

	Entity* Tea3 = new Entity_Fluid(true, 0.0f, 0.0f);
	Tea3->names = { "tea" };
	Tea3->SetParent(Inside, Cup3);
	Tea3->AddAdjective(Taste, "smooth");
	Tea3->AddAdjective(Visual, "green");
	entities.push_back(Tea3);
	
	Entity_Readable* Note = new Entity_Readable(true, 0.0f, 0.0f);
	Note->requiredLanguage = English;
	Note->text = "This is a note.";
	Note->names = { "note" };
	Note->SetParent(On, Table);
	entities.push_back(Note);

}
