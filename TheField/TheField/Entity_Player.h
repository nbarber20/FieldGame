#pragma once
#include "Entity_Human.h"
#include <iostream>
#include <istream>
#include <string>
class Entity_Player : public Entity_Human
{
public:
	Entity_Player(bool visibleInsides, float internalVolume, float size) :Entity_Human(visibleInsides, internalVolume, size)
	{
		individualName = "Nick";
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	}

	virtual void Tick() override
	{
		std::vector<Entity*> inMouth = GetInventory(Mouth);
		for (int i = 0; i < inMouth.size(); i++) {
			TrySwallow(inMouth[i]);
		}
	}

	virtual Entity_Player* Clone() {
		return new Entity_Player(*this);
	}
	virtual std::vector<Entity*>  getVisibleEntities();
	virtual Entity* getNearbyEntity(std::string entityName);
	virtual void Look();
	virtual Entity* FindEntityByName(std::string entityName);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective, std::vector<Position> positionBlacklist);
	virtual bool TryMove(Entity* e, Position toPos, Entity* toEntity);
	virtual bool Enter(Entity* e);
	virtual bool Exit(Entity* e);
	virtual bool Drink(Entity* e);
	virtual bool TrySwallow(Entity* e);


	int strength = 10;
};

