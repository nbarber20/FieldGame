#include "pch.h"
#include "World.h"
#include "Entity.h"
#include "Entity_GroundTile.h"
#include "Entity_Player.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "Entity_Readable.h"
#include "Entity_Interior.h"

World::World()
{
}

void World::AddEntity(Entity* e)
{
	entities.push_back(e);
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

	playerEntity = new Entity_Player(false, 0.0f, 48.0f);
	playerEntity->names = { "you" };
	playerEntity->SetParent(On, Ground);
	entities.push_back(playerEntity);

	Entity_Interior* House = new Entity_Interior(false, 47520.0f, 47520.0f);
	House->names = { "house","home"};
	House->SetParent(On, Ground, true);
	House->AddAdjective(Visual, "brick");
	House->AddAdjective(Inside, "cozy");
	House->AddRoom();
	entities.push_back(House);

	Entity* Table = new Entity(true, 0.0f, 50.0f);
	Table->names = { "table" };
	Table->SetParent(OnFloor, House);
	entities.push_back(Table);

	Entity* Cup = new Entity_Container(true, 14.44f, 14.50f);
	Cup->names = { "cup" };
	Cup->SetParent(On, Table);
	Cup->AddAdjective(Visual, "wooden");
	entities.push_back(Cup);

	Entity* Tea = new Entity_Fluid(true, 0.0f, 0.0f);
	Tea->names = { "tea" };
	Tea->SetParent(Inside, Cup);
	Tea->AddAdjective(Taste, "bitter");
	Tea->AddAdjective(Visual, "black");
	entities.push_back(Tea);

	Entity* Cup2 = new Entity_Container(true, 14.44f, 14.50f);
	Cup2->names = { "cup", "container" };
	Cup2->SetParent(On, Table);
	Cup2->AddAdjective(Visual, "ugly");
	Cup2->AddAdjective(NameExtension, "liquid");
	entities.push_back(Cup2);

	Entity* Cup3 = new Entity_Container(true, 14.44f, 14.50f);
	Cup3->names = { "cup" };
	Cup3->SetParent(OnFloor, House);
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
